===============================
Persistence in Hippo and Phongo
===============================

This document discusses the methods how compound structures (documents,
arrays, objects) are persisted through the drivers. And how they are brought
back into PHP land.

Serialization to BSON
=====================

Arrays
------

If an array is a *packed array* — i.e. the keys start at 0 and are sequential
without gaps: **BSON array**. 

If the array is not packed — i.e. having associative (string) keys, the keys
don't start at 0, or when they are gaps:: **BSON object**

Examples
~~~~~~~~

These serialize as a BSON array::

  [ 8, 5, 2, 3 ] => [ 8, 5, 2, 3 ]
  [ 0 => 4, 1 => 9 ] => [ 4, 9 ]


These serialize as a BSON document::

  [ 0 => 1, 2 => 8, 3 => 12 ] => { "0" : 1, "2" : 8, "3" : 12 }
  [ "foo" => 42 ] => { "foo" : 42 }

To discuss:
 
- This is one is tricky, as the internal order in PHP does not have sequential
  arrays::

    [ 1 => 9, 0 => 10 ] => { "1" : 9, "0" => 10 }

  I suggest we serialize this as a **BSON document**.


Objects
-------

If an object is of the *stdClass*, serialize as a **BSON document**.

If an object is of any other class, without implementing any special
interface, serialize as a **BSON document**. Keep only *public* properties,
and ignore *protected* and *private* properties.

If an object is of a class that implements the ``MongoDB\BSON\Serializable``
interface, call ``bsonSerialize`` and used the returned associative array to
store as properties of a **BSON document**. Not returning an array is an error
and should throw an ``MongoDB\Driver\Exception\UnexpectedValueException``
exception. It is valid to return a packed array, but it must also be stored as
a **BSON document**.

If an object is of a class that implements the ``MongoDB\BSON\Persistable``
interface (which implies ``MongoDB\BSON\Serializable``), obtain the properties
in a similar way as in the previous paragraph, but *also* add an additional
property ``__pclass`` as a Binary value, with subtype ``0x80`` and as value
the fully qualified class name of the object that is being serialized.

Examples
~~~~~~~~

::
  stdClass {
    public $foo => 42
  } => { 'foo' : 42 }

  MyClass {
    public $foo => 42,
    protected $prot => "wine",
    private $fpr => "cheese"
  } => { 'foo' : 42 }

  AnotherClass implements Serializable {
    public $foo => 42,
    protected $prot => "wine",
    private $fpr => "cheese"
    function bsonSerialize() : array {
        return [ 'foo' => $this->foo, 'prot' => $this->prot ];
    }
  } => { 'foo' : 42, 'prot' : "wine" }

  UpperClass implements Persistable {
    public $foo => 42,
    protected $prot => "wine",
    private $fpr => "cheese"
    function bsonSerialize() : array {
        return [ 'foo' => $this->foo, 'prot' => $this->prot ];
    }
  } => { 'foo' : 42, 'prot' : "wine", '__pclass' : Binary(0x80, "UpperClass") }


Deserialization from BSON
=========================

For compound types, there are three data types:

- ``root``: refers to the top-level BSON document *only*
- ``document``: refers to nested BSON document *only*
- ``array``: refers to a BSON array

Each of those three data types can be mapped against different PHP types. The
possible mapping values are:

- *not set* or ``NULL`` — this is the default.

  - A BSON array will be deserialized as a PHP ``array``.
  - A BSON document (root or nested) without ``__pclass`` property[1]_ becomes a
    PHP ``stdClass`` object, with each BSON document property becoming a
    public ``stdClass`` property.
  - A BSON document (root or nested) with ``__pclass`` property[1]_ becomes
    a PHP object of the class name as defined by the ``__pclass`` property.

    If the named class implements the ``MongoDB\BSON\Unserializable``
    interface, then the properties of the BSON document, minus the
    ``__pclass`` property are sent as an associative array to the
    ``bsonUnserialize`` function to initialise the object's properties.
    
    If the named class does not implement the ``MongoDB\BSON\Unserializable``
    interface, then the object gets initialized with the properties
    **without** the ``__pclass`` property being one of them. It simply gets
    left out.

    The rationale for the latter is because no explicit choice about whether
    to use ODM features has been made, it should not bail out because of an
    extra ``__pclass`` property. The behaviour is different if the data type
    was mapped to ``"odm"`` (see below).

- ``"array"`` — turns a BSON array or BSON document into a PHP array.
  ``__pclass`` properties[1]_ are igored.

- ``"object"`` or ``"stdClass"`` — turns a BSON array or BSON document into a
  ``stdClass`` object. There will be no special treatment of a ``__pclass``
  property[1]_, buy it should **not** be set as property in the returned
  object.

- ``"odm"`` — uses the class name set in the ``__pclass`` property of a BSON
  array or BSON document to determine the class name of the created object.

  The properties of the BSON document, minus the ``__pclass`` property are
  sent as an associative array to the ``bsonUnserialize`` function to initialise
  the object's properties.

  Remarks:

  - If the ``__pclass`` property is not present[1]_, this throws the
    ``MongoDB\Driver\Exception\UnexpectedValueException`` exception with a
    message indicating the special class marker could not be found.

  - If the class as determined by the ``__pclass`` property does not implement
    the ``MongoDB\BSON\Persistable`` interface, throw an
    ``MongoDB\Driver\Exception\UnexpectedValueException`` exception with a
    message indicating that the named class does not implement the required
    interface.

- ``any other string`` — defines the class name that the BSON array or BSON
  object should be deserialized at.

  If the class implements the ``MongoDB\BSON\Unserializable`` interface, then
  the properties of the BSON document, **minus** the ``__pclass`` property[1]_
  if it exists, are sent as an associative array to the ``bsonUnserialize``
  function to initialise the object's properties.

  To discuss:

  - What should we do if the named class implements
    ``MongoDB\BSON\Persistable``?
    
    We can throw an ``MongoDB\Driver\Exception\UnexpectedValueException``
    exception indicating that the BSON document really knows what PHP type it
    should be. This could be important if the object needs to be deserialized
    in a special way.

    We can ignore it and act as the ``__pclass`` property wasn't set at all.
    And not send the ``__pclass`` property as an array element to
    ``bsonUnserialize``.

TypeMaps
--------

TypeMaps can be set through the ``setTypeMap()`` on a
``MongoDB\Driver\Cursor`` object, or the ``$typeMap`` argument of
``MongoDB\BSON\toPHP()`` (previously, ``MongoDB\BSON\toArray()``). Each of the
three classes (``root``, ``document`` and ``array``) can be individually set.
If ``document`` is set to something else than the default and ``root`` is set
to the default (or not set at all), then the value of ``document`` also counts
for ``root``. Which makes the following example equivalent::

    ->setTypeMap( [ 'document' => 'MyClass' ] )
    ->setTypeMap( [ 'document' => 'MyClass', 'root' => 'MyClass' ] );
    ->setTypeMap( [ 'document' => 'MyClass', 'root' => NULL ] );

But these are **not** equivalent::

    ->setTypeMap( [ 'root' => 'MyClass' ] )
    ->setTypeMap( [ 'document' => 'MyClass', 'root' => 'MyClass' ] );

In the first line of this second example, the value for ``document`` stays
the default, which is ``stdClass``.

Examples
--------

In these examples, ``MyClass`` does **not** implement any interface,
``YourClass`` implements ``MongoDB\BSON\Unserializable`` and ``OurClass``
implements ``MongoDB\BSON\Persistable``. Their ``bsonUnserialize()`` methods
iterate over the array and set the properties without modifications. It
**also** sets the ``unserialized = true`` property.

::

    /* typemap: [] (all defaults) */
    { foo: 'yes', 'bar' : false }
      -> stdClass { $foo => 'yes', $bar => false }

    { foo: 'no', 'array' : [ 5, 6 ] }
      -> stdClass { $foo => 'no', $array => [ 5, 6 ] }

    { foo: 'no', 'obj' : { 'nested' => 3.14 } }
      -> stdClass { $foo => 'no', $obj => stdClass { $nested => 3.14 } }

    { foo: 'yes', '__pclass': 'MyClass' }
      -> stdClass { $foo => 'yes', $__pclass => 'MyClass' }

    { foo: 'yes', '__pclass': Binary(0x80, 'MyClass') }
      -> MyClass { $foo => 'yes' }

    { foo: 'yes', '__pclass': Binary(0x80, 'YourClass') }
      -> MyClass { $foo => 'yes', $unserialized => true }

::

    /* typemap: [ 'document' => 'array' ] (also implicitly sets root=array) */
    { foo: 'yes', 'bar' : false }
      -> [ 'foo' => 'yes', 'bar' => false ]

    { foo: 'no', 'array' : [ 5, 6 ] }
      -> [ 'foo' => 'no', 'array' => [ 5, 6 ] ]

    { foo: 'no', 'obj' : { 'nested' => 3.14 } }
      -> [ 'foo' => 'no', 'obj' => [ 'nested => 3.14 ] ]

    { foo: 'yes', '__pclass': 'MyClass' }
      -> [ 'foo' => 'yes', '__pclass' => 'MyClass' }

    { foo: 'yes', '__pclass': Binary(0x80, 'MyClass') }
      -> [ 'foo' => 'yes' ]

    { foo: 'yes', '__pclass': Binary(0x80, 'OurClass') }
      -> [ 'foo' => 'yes' ] /* 'unserialized' does not get set, because it's an array */

:: 

    /* typemap: [ 'document' => 'odm' ] (also implicitly sets root=odm) */
    { foo: 'yes', 'bar' : false }
      -> MongoDB\Driver\Exception\UnexpectedValueException("pclass not set")

    { foo: 'no', 'array' : [ 5, 6 ] }
      -> MongoDB\Driver\Exception\UnexpectedValueException("pclass not set")

    { foo: 'no', 'obj' : { 'nested' => 3.14 } }
      -> MongoDB\Driver\Exception\UnexpectedValueException("pclass not set")

    { foo: 'yes', '__pclass': 'MyClass' }
      -> MongoDB\Driver\Exception\UnexpectedValueException("pclass not set")

    { foo: 'yes', '__pclass': Binary(0x80, 'MyClass') }
      -> MongoDB\Driver\Exception\UnexpectedValueException("not persistable")

    { foo: 'yes', '__pclass': Binary(0x80, 'OurClass') }
      -> OurClass { $foo => 'yes', $unserialized => true }


.. [1] A ``__pclass`` property if only deemed to exist if there exists a
   property with that name,  **and** it is a Binary value, **and** the
   sub-type of the Binary value is ``0x80``. If any of these three conditions
   is not met, the ``__pclass`` property does not exist and should be treated
   as any other normal property.
