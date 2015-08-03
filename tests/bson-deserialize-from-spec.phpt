--TEST--
BSON deserialization: examples from spec
--FILE--
<?php
trait tBsonUnserialize
{
	function bsonUnserialize( array $map )
	{
		foreach ( $map as $k => $value )
		{
			$this->$k = $value;
		}
		$this->unserialized = true;
	}
}

trait tBsonSerialize
{
	function bsonSerialize()
	{
		/* Not tested with this test, so return empty array */
		return [];
	}
}

class MyClass
{

}

class YourClass implements MongoDB\BSON\Unserializable
{
	use tBsonUnserialize;
}

class OurClass implements MongoDB\BSON\Persistable
{
	use tBsonUnserialize, tBsonSerialize;
}

class TheirClass extends OurClass
{
}

$bMyClass = base64_encode( "MyClass" );
$bYourClass = base64_encode( "YourClass" );
$bOurClass = base64_encode( "OurClass" );
$bTheirClass = base64_encode( "TheirClass" );
$bInterface = base64_encode( "MongoDB\BSON\Unserializable" );

$tests = [
	[
		'name' => 'DEFAULT TYPEMAP',
		'typemap' => [],
		'tests' => [
			'{ "foo": "yes", "bar" : false }',
			'{ "foo": "no", "array" : [ 5, 6 ] }',
			'{ "foo": "no", "obj" : { "embedded" : 3.14 } }',
			'{ "foo": "yes", "__pclass": "MyClass" }',
			'{ "foo": "yes", "__pclass": { "$type" : "80", "$binary" : "' . $bMyClass . '" } }',
			'{ "foo": "yes", "__pclass": { "$type" : "80", "$binary" : "' . $bYourClass . '" } }',
			'{ "foo": "yes", "__pclass": { "$type" : "80", "$binary" : "' . $bOurClass . '" } }',
			'{ "foo": "yes", "__pclass": { "$type" : "44", "$binary" : "' . $bYourClass . '" } }',
		]
	],
	[
		'name' => 'NONEXISTING CLASS',
		'typemap' => [ 'root' => 'MissingClass' ],
		'tests' => [
			'{ "foo": "yes" }',
		]
	],
	[
		'name' => "DOES NOT IMPLEMENT UNSERIALIZABLE",
		'typemap' => [ 'root' => 'MyClass' ],
		'tests' => [
			'{ "foo": "yes", "__pclass": { "$type": "80", "$binary": "' . $bMyClass . '" } }'
		]
	],
	[
		'name' => "IS NOT A CONCRETE CLASS",
		'typemap' => [ 'root' => 'MongoDB\BSON\Unserializable' ],
		'tests' => [
			'{ "foo": "yes" }',
		]
	],
	[
		'name' => "IS NOT A CONCRETE CLASS VIA PCLASS",
		'typemap' => [ 'root' => 'YourClass' ],
		'tests' => [
			'{ "foo": "yes", "__pclass" : { "$type": "80", "$binary": "' . $bInterface . '" } }',
		]
	],
	[
		'name' => 'PCLASS OVERRIDES TYPEMAP (1)',
		'typemap' => [ 'root' => 'YourClass' ],
		'tests' => [
			'{ "foo": "yes", "__pclass" : { "$type": "80", "$binary": "' . $bMyClass . '" } }',
			'{ "foo": "yes", "__pclass" : { "$type": "80", "$binary": "' . $bOurClass . '" } }',
			'{ "foo": "yes", "__pclass" : { "$type": "80", "$binary": "' . $bTheirClass . '" } }',
			'{ "foo": "yes", "__pclass" : { "$type": "80", "$binary": "' . $bYourClass . '" } }',
		]
	],
	[
		'name' => 'PCLASS OVERRIDES TYPEMAP (2)',
		'typemap' => [ 'root' => 'OurClass' ],
		'tests' => [
			'{ "foo": "yes", "__pclass" : { "$type": "80", "$binary": "' . $bTheirClass . '" } }',
		]
	],
	[
		'name' => 'OBJECTS AS ARRAY',
		'typemap' => [ 'root' => 'array', 'document' => 'array' ],
		'tests' => [
			'{ "foo": "yes", "bar" : false }',
			'{ "foo": "no", "array" : [ 5, 6 ] }',
			'{ "foo": "no", "obj" : { "embedded" : 3.14 } }',
			'{ "foo": "yes", "__pclass": "MyClass" }',
			'{ "foo": "yes", "__pclass" : { "$type": "80", "$binary": "' . $bMyClass . '" } }',
			'{ "foo": "yes", "__pclass" : { "$type": "80", "$binary": "' . $bOurClass . '" } }',
		]
	],
	[
		'name' => 'OBJECTS AS STDCLASS',
		'typemap' => [ 'root' => 'object', 'documenmt' => 'object' ],
		'tests' => [
			'{ "foo": "yes", "__pclass" : { "$type": "80", "$binary": "' . $bMyClass . '" } }',
		]
	]
];

foreach ( $tests as $testGroup )
{
	echo "=== {$testGroup['name']} ===\n\n";
	foreach ( $testGroup['tests'] as $test )
	{
		echo $test, "\n";
		$bson = MongoDB\BSON\fromJSON( $test );
		try
		{
			var_dump( MongoDB\BSON\toPHP( $bson, $testGroup[ 'typemap' ] ) );
		}
		catch ( Exception $e )
		{
			echo $e->getMessage(), "\n";
		}
		echo "\n";
	}
	echo "\n";
}
?>
--EXPECT--
=== DEFAULT TYPEMAP ===

{ "foo": "yes", "bar" : false }
object(stdClass)#1 (2) {
  ["foo"]=>
  string(3) "yes"
  ["bar"]=>
  bool(false)
}

{ "foo": "no", "array" : [ 5, 6 ] }
object(stdClass)#1 (2) {
  ["foo"]=>
  string(2) "no"
  ["array"]=>
  array(2) {
    [0]=>
    int(5)
    [1]=>
    int(6)
  }
}

{ "foo": "no", "obj" : { "embedded" : 3.14 } }
object(stdClass)#2 (2) {
  ["foo"]=>
  string(2) "no"
  ["obj"]=>
  object(stdClass)#1 (1) {
    ["embedded"]=>
    float(3.14)
  }
}

{ "foo": "yes", "__pclass": "MyClass" }
object(stdClass)#2 (2) {
  ["foo"]=>
  string(3) "yes"
  ["__pclass"]=>
  string(7) "MyClass"
}

{ "foo": "yes", "__pclass": { "$type" : "80", "$binary" : "TXlDbGFzcw==" } }
object(stdClass)#4 (2) {
  ["foo"]=>
  string(3) "yes"
  ["__pclass"]=>
  object(MongoDB\BSON\Binary)#2 (2) {
    ["data"]=>
    string(7) "MyClass"
    ["subType"]=>
    int(128)
  }
}

{ "foo": "yes", "__pclass": { "$type" : "80", "$binary" : "WW91ckNsYXNz" } }
object(stdClass)#6 (2) {
  ["foo"]=>
  string(3) "yes"
  ["__pclass"]=>
  object(MongoDB\BSON\Binary)#4 (2) {
    ["data"]=>
    string(9) "YourClass"
    ["subType"]=>
    int(128)
  }
}

{ "foo": "yes", "__pclass": { "$type" : "80", "$binary" : "T3VyQ2xhc3M=" } }
object(OurClass)#7 (3) {
  ["foo"]=>
  string(3) "yes"
  ["__pclass"]=>
  object(MongoDB\BSON\Binary)#6 (2) {
    ["data"]=>
    string(8) "OurClass"
    ["subType"]=>
    int(128)
  }
  ["unserialized"]=>
  bool(true)
}

{ "foo": "yes", "__pclass": { "$type" : "44", "$binary" : "WW91ckNsYXNz" } }
object(stdClass)#9 (2) {
  ["foo"]=>
  string(3) "yes"
  ["__pclass"]=>
  object(MongoDB\BSON\Binary)#8 (2) {
    ["data"]=>
    string(9) "YourClass"
    ["subType"]=>
    int(68)
  }
}


=== NONEXISTING CLASS ===

{ "foo": "yes" }
The typemap does not provide a class that implements MongoDB\BSON\Unserializable


=== DOES NOT IMPLEMENT UNSERIALIZABLE ===

{ "foo": "yes", "__pclass": { "$type": "80", "$binary": "TXlDbGFzcw==" } }
The typemap does not provide a class that implements MongoDB\BSON\Unserializable


=== IS NOT A CONCRETE CLASS ===

{ "foo": "yes" }
The typemap does not provide a class that implements MongoDB\BSON\Unserializable


=== IS NOT A CONCRETE CLASS VIA PCLASS ===

{ "foo": "yes", "__pclass" : { "$type": "80", "$binary": "TW9uZ29EQlxCU09OXFVuc2VyaWFsaXphYmxl" } }
object(YourClass)#14 (3) {
  ["foo"]=>
  string(3) "yes"
  ["__pclass"]=>
  object(MongoDB\BSON\Binary)#13 (2) {
    ["data"]=>
    string(27) "MongoDB\BSON\Unserializable"
    ["subType"]=>
    int(128)
  }
  ["unserialized"]=>
  bool(true)
}


=== PCLASS OVERRIDES TYPEMAP (1) ===

{ "foo": "yes", "__pclass" : { "$type": "80", "$binary": "TXlDbGFzcw==" } }
object(YourClass)#15 (3) {
  ["foo"]=>
  string(3) "yes"
  ["__pclass"]=>
  object(MongoDB\BSON\Binary)#14 (2) {
    ["data"]=>
    string(7) "MyClass"
    ["subType"]=>
    int(128)
  }
  ["unserialized"]=>
  bool(true)
}

{ "foo": "yes", "__pclass" : { "$type": "80", "$binary": "T3VyQ2xhc3M=" } }
object(OurClass)#16 (3) {
  ["foo"]=>
  string(3) "yes"
  ["__pclass"]=>
  object(MongoDB\BSON\Binary)#15 (2) {
    ["data"]=>
    string(8) "OurClass"
    ["subType"]=>
    int(128)
  }
  ["unserialized"]=>
  bool(true)
}

{ "foo": "yes", "__pclass" : { "$type": "80", "$binary": "VGhlaXJDbGFzcw==" } }
object(TheirClass)#17 (3) {
  ["foo"]=>
  string(3) "yes"
  ["__pclass"]=>
  object(MongoDB\BSON\Binary)#16 (2) {
    ["data"]=>
    string(10) "TheirClass"
    ["subType"]=>
    int(128)
  }
  ["unserialized"]=>
  bool(true)
}

{ "foo": "yes", "__pclass" : { "$type": "80", "$binary": "WW91ckNsYXNz" } }
object(YourClass)#18 (3) {
  ["foo"]=>
  string(3) "yes"
  ["__pclass"]=>
  object(MongoDB\BSON\Binary)#17 (2) {
    ["data"]=>
    string(9) "YourClass"
    ["subType"]=>
    int(128)
  }
  ["unserialized"]=>
  bool(true)
}


=== PCLASS OVERRIDES TYPEMAP (2) ===

{ "foo": "yes", "__pclass" : { "$type": "80", "$binary": "VGhlaXJDbGFzcw==" } }
object(TheirClass)#19 (3) {
  ["foo"]=>
  string(3) "yes"
  ["__pclass"]=>
  object(MongoDB\BSON\Binary)#18 (2) {
    ["data"]=>
    string(10) "TheirClass"
    ["subType"]=>
    int(128)
  }
  ["unserialized"]=>
  bool(true)
}


=== OBJECTS AS ARRAY ===

{ "foo": "yes", "bar" : false }
array(2) {
  ["foo"]=>
  string(3) "yes"
  ["bar"]=>
  bool(false)
}

{ "foo": "no", "array" : [ 5, 6 ] }
array(2) {
  ["foo"]=>
  string(2) "no"
  ["array"]=>
  array(2) {
    [0]=>
    int(5)
    [1]=>
    int(6)
  }
}

{ "foo": "no", "obj" : { "embedded" : 3.14 } }
array(2) {
  ["foo"]=>
  string(2) "no"
  ["obj"]=>
  array(1) {
    ["embedded"]=>
    float(3.14)
  }
}

{ "foo": "yes", "__pclass": "MyClass" }
array(2) {
  ["foo"]=>
  string(3) "yes"
  ["__pclass"]=>
  string(7) "MyClass"
}

{ "foo": "yes", "__pclass" : { "$type": "80", "$binary": "TXlDbGFzcw==" } }
array(2) {
  ["foo"]=>
  string(3) "yes"
  ["__pclass"]=>
  object(MongoDB\BSON\Binary)#19 (2) {
    ["data"]=>
    string(7) "MyClass"
    ["subType"]=>
    int(128)
  }
}

{ "foo": "yes", "__pclass" : { "$type": "80", "$binary": "T3VyQ2xhc3M=" } }
array(2) {
  ["foo"]=>
  string(3) "yes"
  ["__pclass"]=>
  object(MongoDB\BSON\Binary)#20 (2) {
    ["data"]=>
    string(8) "OurClass"
    ["subType"]=>
    int(128)
  }
}


=== OBJECTS AS STDCLASS ===

{ "foo": "yes", "__pclass" : { "$type": "80", "$binary": "TXlDbGFzcw==" } }
object(stdClass)#22 (2) {
  ["foo"]=>
  string(3) "yes"
  ["__pclass"]=>
  object(MongoDB\BSON\Binary)#21 (2) {
    ["data"]=>
    string(7) "MyClass"
    ["subType"]=>
    int(128)
  }
}
