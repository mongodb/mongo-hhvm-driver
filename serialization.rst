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
don't start at 0, or when there are gaps:: **BSON object**

Examples
~~~~~~~~

These serialize as a BSON array::

  [ 8, 5, 2, 3 ] => [ 8, 5, 2, 3 ]
  [ 0 => 4, 1 => 9 ] => [ 4, 9 ]


These serialize as a BSON document::

  [ 0 => 1, 2 => 8, 3 => 12 ] => { "0" : 1, "2" : 8, "3" : 12 }
  [ "foo" => 42 ] => { "foo" : 42 }
  [ 1 => 9, 0 => 10 ] => { "1" : 9, "0" => 10 }

Objects
-------

If an object is of the *stdClass*, serialize as a **BSON document**.

If an object is of any other class, without implementing any special
interface, serialize as a **BSON document**. Keep only *public* properties,
and ignore *protected* and *private* properties.

If an object is of a class that implements the ``MongoDB\BSON\Serializable``
interface, call ``bsonSerialize`` and use the returned array or ``stdClass`` to
serialize as a BSON document or array. The BSON type will be determined by the
following:

1. Root documents must be serialized as a BSON document.
2. ``MongoDB\BSON\Persistable`` objects must be serialized as a BSON document.
3. If ``bsonSerialize`` returns a packed array, serialize as a BSON array.
4. If ``bsonSerialize`` returns a non-packed array or ``stdClass``, serialize as
   a BSON document.
5. If ``bsonSerialize`` did not return an array or ``stdClass``, throw an
   ``MongoDB\Driver\Exception\UnexpectedValueException`` exception.

If an object is of a class that implements the ``MongoDB\BSON\Persistable``
interface (which implies ``MongoDB\BSON\Serializable``), obtain the properties
in a similar way as in the previous paragraph, but *also* add an additional
property ``__pclass`` as a Binary value, with subtype ``0x80`` and data bearing
the fully qualified class name of the object that is being serialized.

The ``__pclass`` property is added to the array or object returned by
``bsonSerialize``, which means it will overwrite any ``__pclass`` key/property
in the ``bsonSerialize`` return value. If you want to avoid this behaviour and
set your own ``__pclass`` value, you must **not** implement
``MongoDB\BSON\Persistable`` and should instead implement
``MongoDB\BSON\Serializable`` directly.

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

  AnotherClass implements MongoDB\BSON\Serializable {
    public $foo => 42,
    protected $prot => "wine",
    private $fpr => "cheese"
    function bsonSerialize() {
        return [ 'foo' => $this->foo, 'prot' => $this->prot ];
    }
  } => { 'foo' : 42, 'prot' : "wine" }

  AnotherClass implements MongoDB\BSON\Serializable {
    public $foo => 42
    function bsonSerialize() {
        return $this;
    }
  } => MongoDB\Driver\Exception\UnexpectedValueException("bsonSerialize() did not return an array or stdClass")

  AnotherClass implements MongoDB\BSON\Serializable {
    private $elements => [ 'foo', 'bar' ]
    function bsonSerialize() {
        return $this->elements;
    }
  } => { '0' : 'foo', '1' : 'bar' }

  ContainerClass implements MongoDB\BSON\Serializable {
    public $things => AnotherClass implements MongoDB\BSON\Serializable {
      private $elements => [ 0 => 'foo', 2 => 'bar' ]
      function bsonSerialize() {
        return $this->elements;
      }
    }
    function bsonSerialize() {
        return [ 'things' => $this->things ];
    }
  } => { 'things' : { '0' : 'foo', '2' : 'bar' } }

  ContainerClass implements MongoDB\BSON\Serializable {
    public $things => AnotherClass implements MongoDB\BSON\Serializable {
      private $elements => [ 0 => 'foo', 2 => 'bar' ]
      function bsonSerialize() {
        return array_values($this->elements);
      }
    }
    function bsonSerialize() {
        return [ 'things' => $this->things ];
    }
  } => { 'things' : [ 'foo', 'bar' ] }

  ContainerClass implements MongoDB\BSON\Serializable {
    public $things => AnotherClass implements MongoDB\BSON\Serializable {
      private $elements => [ 'foo', 'bar' ]
      function bsonSerialize() {
        return (object) $this->elements;
      }
    }
    function bsonSerialize() {
        return [ 'things' => $this->things ];
    }
  } => { 'things' : { '0' : 'foo', '1' : 'bar' } }

  UpperClass implements MongoDB\BSON\Persistable {
    public $foo => 42,
    protected $prot => "wine",
    private $fpr => "cheese"
    function bsonSerialize() {
        return [ 'foo' => $this->foo, 'prot' => $this->prot ];
    }
  } => { 'foo' : 42, 'prot' : "wine", '__pclass' : MongoDB\BSON\Binary(0x80, "UpperClass") }


Deserialization from BSON
=========================

For compound types, there are three data types:

- ``root``: refers to the top-level BSON document *only*
- ``document``: refers to embedded BSON documents *only*
- ``array``: refers to a BSON array

Each of those three data types can be mapped against different PHP types. The
possible mapping values are:

- *not set* or ``NULL`` — this is the default.

  - A BSON array will be deserialized as a PHP ``array``.
  - A BSON document (root or embedded) without ``__pclass`` property [1]_ becomes a
    PHP ``stdClass`` object, with each BSON document key becoming a
    public ``stdClass`` property.
  - A BSON document (root or embedded) with ``__pclass`` property [1]_ becomes
    a PHP object of the class name as defined by the ``__pclass`` property.

    If the named class implements the ``MongoDB\BSON\Unserializable``
    interface, then the properties of the BSON document, including the
    ``__pclass`` property, are sent as an associative array to the
    ``bsonUnserialize`` function to initialise the object's properties.
    
    If the named class does not implement the ``MongoDB\BSON\Unserializable``
    interface, then an ``MongoDB\Driver\UnexpectedValueException`` exception
    is thrown with a message indicating that the class does not implement the
    expected interface.

- ``"array"`` — turns a BSON array or BSON document into a PHP array.
  ``__pclass`` properties [1]_ are ignored, and are **not** set as an array
  element in the returned array.

- ``"object"`` or ``"stdClass"`` — turns a BSON array or BSON document into a
  ``stdClass`` object. There will be no special treatment of a ``__pclass``
  property [1]_, but it will **not** be set as property in the returned
  object.

- ``any other string`` — defines the class name that the BSON array or BSON
  object should be deserialized as.

  If the class implements the ``MongoDB\BSON\Unserializable`` interface,
  either directly or indirectly via ``MongoDB\BSON\Persistable``, then
  the properties of the BSON document, **including** the ``__pclass`` property
  if it exists, are sent as an associative array to the ``bsonUnserialize``
  function to initialise the object's properties.

  If the class does not implement the ``MongoDB\BSON\Unserializable``
  interface, then an ``MongoDB\Driver\Exception\UnexpectedValueException``
  exception is thrown.

  If the named class is different from the ``__pclass`` key's value, then the
  ``__pclass`` value is ignored and the class name from the type map is used.
  The properties of the BSON document (including ``__pclass``) are sent to
  ``bsonUnserialize`` as per above.

TypeMaps
--------

TypeMaps can be set through the ``setTypeMap()`` method on a
``MongoDB\Driver\Cursor`` object, or the ``$typeMap`` argument of
``MongoDB\BSON\toPHP()`` (previously, ``MongoDB\BSON\toArray()``). Each of the
three classes (``root``, ``document`` and ``array``) can be individually set.

If the value in the map is ``NULL``, it means the same as the *default* value
for that item.

Examples
--------

In these examples, ``MyClass`` does **not** implement any interface,
``YourClass`` implements ``MongoDB\BSON\Unserializable`` and ``OurClass``
implements ``MongoDB\BSON\Persistable``.

The ``bsonUnserialize()`` method of ``YourClass`` and ``OurClass``
iterate over the array and set the properties without modifications. It
**also** sets the ``$unserialized`` property to ``true``::

    function bsonUnserialize( array $map )
    {
        foreach ( $map as $k => $value )
        {
            $this->$k = $value;
        }
        $this->unserialized = true;
    }

::

    /* typemap: [] (all defaults) */
    { foo: 'yes', 'bar' : false }
      -> stdClass { $foo => 'yes', $bar => false }

    { foo: 'no', 'array' : [ 5, 6 ] }
      -> stdClass { $foo => 'no', $array => [ 5, 6 ] }

    { foo: 'no', 'obj' : { 'embedded' => 3.14 } }
      -> stdClass { $foo => 'no', $obj => stdClass { $embedded => 3.14 } }

    { foo: 'yes', '__pclass': 'MyClass' }
      -> stdClass { $foo => 'yes', $__pclass => 'MyClass' }

    { foo: 'yes', '__pclass': Binary(0x80, 'MyClass') }
      -> MongoDB\Driver\Exception\UnexpectedValueException("class does not implement unserializable interface")

    { foo: 'yes', '__pclass': Binary(0x80, 'YourClass') }
      -> MyClass { $foo => 'yes', $__pclass => Binary(0x80, 'YourClass'), $unserialized => true }

    { foo: 'yes', '__pclass': Binary(0x44, 'YourClass') }
      -> stdClass { $foo => 'yes', $__pclass => Binary(0x44, 'YourClass') }

::

    /* typemap: [ 'root' => 'MyClass' ] */
    { foo: 'yes', '__pclass' => Binary(0x80, 'MyClass') }
      -> MongoDB\Driver\Exception\UnexpectedValueException("class does not implement unserializable interface")

::

    /* typemap: [ 'root' => 'YourClass' ] */
    { foo: 'yes', '__pclass' => Binary(0x80, 'YourClass') }
      -> YourClass { $foo => 'yes', $__pclass => Binary(0x80, 'YourClass'), $unserialized => true }

::

    /* typemap: [ 'root' => 'array', 'document' => 'array' ] */
    { foo: 'yes', 'bar' : false }
      -> [ 'foo' => 'yes', 'bar' => false ]

    { foo: 'no', 'array' : [ 5, 6 ] }
      -> [ 'foo' => 'no', 'array' => [ 5, 6 ] ]

    { foo: 'no', 'obj' : { 'embedded' => 3.14 } }
      -> [ 'foo' => 'no', 'obj' => [ 'embedded => 3.14 ] ]

    { foo: 'yes', '__pclass': 'MyClass' }
      -> [ 'foo' => 'yes', '__pclass' => 'MyClass' ]

    { foo: 'yes', '__pclass': Binary(0x80, 'MyClass') }
      -> [ 'foo' => 'yes' ]

    { foo: 'yes', '__pclass': Binary(0x80, 'OurClass') }
      -> [ 'foo' => 'yes' ] /* 'unserialized' does not get set, because it's an array */

::

    /* typemap: [ 'root' => 'object', 'document' => 'object' ] */
    { foo: 'yes', '__pclass': Binary(0x80, 'MyClass') }
      -> stdClass { $foo => 'yes' } /* 'unserialized' does not get set, because it's a stdClass */


Related Tickets
===============

- PHPC-248_: Allow ->setTypeMap() to set 'array' and 'stdclass'
- PHPC-249_: empty array should be serialized as array
- PHPC-260_: Allow "object" as an alias of "stdClass" for setTypeMap()
- PHPC-274_: zval_to_bson() ignores BSON\Serializable interface
- PHPC-275_: object_to_bson() should throw exception if bsonSerialize()
  returns non-array
- PHPC-288_: ODS (Object Document Serializer) support and integrations
- PHPC-311_: Rename BSON from/toArray() methods to from/toPHP()
- PHPC-315_: Support explicit type mapping for top-level documents
- PHPC-318_: Cursor type map should apply to top-level document
- PHPC-319_: Top level documents should be deserialized as stdClass by default
- PHPC-329_: Determine if ODM class should always supersede the type map

- HHVM-55_: Implement BSON\Peristable interface
- HHVM-56_: Implement BSON\Serializable interface
- HHVM-57_: Implement BSON\Unserializable interface
- HHVM-63_: Empty array should be serialized as empty array, and empty object
  should be serialized as empty object
- HHVM-64_: Allow ->setTypeMap() to set 'array' and 'stdclass'
- HHVM-67_: ODM should only match field of specific name (__pclass)
- HHVM-84_: Implement MongoDB\BSON\Serializable
- HHVM-85_: Implement MongoDB\BSON\Unserializable / MongoDB\BSON\Persistable

- PHP-1457_: MongoCollection::insert() Non-public properties of objects.



.. _PHPC-248: https://jira.mongodb.org/browse/PHPC-248
.. _PHPC-249: https://jira.mongodb.org/browse/PHPC-249
.. _PHPC-260: https://jira.mongodb.org/browse/PHPC-260
.. _PHPC-274: https://jira.mongodb.org/browse/PHPC-274
.. _PHPC-275: https://jira.mongodb.org/browse/PHPC-275
.. _PHPC-288: https://jira.mongodb.org/browse/PHPC-288
.. _PHPC-311: https://jira.mongodb.org/browse/PHPC-311
.. _PHPC-315: https://jira.mongodb.org/browse/PHPC-315
.. _PHPC-318: https://jira.mongodb.org/browse/PHPC-318
.. _PHPC-319: https://jira.mongodb.org/browse/PHPC-319
.. _PHPC-329: https://jira.mongodb.org/browse/PHPC-329
.. _HHVM-55: https://jira.mongodb.org/browse/HHVM-55
.. _HHVM-56: https://jira.mongodb.org/browse/HHVM-56
.. _HHVM-57: https://jira.mongodb.org/browse/HHVM-57
.. _HHVM-63: https://jira.mongodb.org/browse/HHVM-63
.. _HHVM-64: https://jira.mongodb.org/browse/HHVM-64
.. _HHVM-67: https://jira.mongodb.org/browse/HHVM-67
.. _HHVM-84: https://jira.mongodb.org/browse/HHVM-84
.. _HHVM-85: https://jira.mongodb.org/browse/HHVM-85
.. _PHP-1457: https://jira.mongodb.org/browse/PHP-1457

Unrelated Tickets
=================

- PHPC-314_: Prototype type map syntax for documents within field paths

.. _PHPC-314: https://jira.mongodb.org/browse/PHPC-314

.. [1] A ``__pclass`` property is only deemed to exist if there exists a
   property with that name,  **and** it is a Binary value, **and** the
   sub-type of the Binary value is ``0x80``. If any of these three conditions
   is not met, the ``__pclass`` property does not exist and should be treated
   as any other normal property.
