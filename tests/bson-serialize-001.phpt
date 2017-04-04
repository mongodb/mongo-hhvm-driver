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
--EXPECTF--
string(%d) "{ "barbar" : 3.141592653589%d, "foo" : { "3" : 42, "1" : false } }"
string(25) "{ "3" : 42, "1" : false }"
