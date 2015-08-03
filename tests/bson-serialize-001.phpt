--TEST--
Test for MongoDB\BSON\Serializable #1
--FILE--
<?php
class Bar
{
	public $barbar = M_PI;
	public $foo;
}

class Foo implements MongoDB\BSON\Serializable
{
	public $bar = 45;

	function bsonSerialize()
	{
		return [ 3 => 42, 1 => false ];
	}
}

$a = new Bar;
$f = new Foo;
$a->foo = $f;

var_dump( MongoDB\BSON\toJson( MongoDB\BSON\fromPHP( $a ) ) );
var_dump( MongoDB\BSON\toJson( MongoDB\BSON\fromPHP( $f ) ) );
?>
--EXPECT--
string(65) "{ "barbar" : 3.1415926535898, "foo" : { "3" : 42, "1" : false } }"
string(25) "{ "3" : 42, "1" : false }"
