--TEST--
Test for MongoDB\BSON\Persistable #1
--FILE--
<?php
class Bar
{
	public $barbar = M_PI;
	public $foo;
}

class Foo implements MongoDB\BSON\Persistable
{
	public $bar = 45;

	function bsonSerialize()
	{
		return [ 'bar' => $this->bar, 'extra' => "Not from a property" ];
	}

	function bsonUnserialize( array $properties )
	{
		foreach ( $properties as $k => $v )
		{
			$prop_name = "prop_{$k}";

			$this->$prop_name = $v;
		}
	}
}

$a = new Bar;
$f = new Foo;
$a->foo = $f;

var_dump( MongoDB\BSON\toArray( MongoDB\BSON\fromArray( $a ) ) );
var_dump( MongoDB\BSON\toArray( MongoDB\BSON\fromArray( $f ) ) );
?>
--EXPECTF--
object(stdClass)#%d (2) {
  ["barbar"]=>
  float(3.1415%d)
  ["foo"]=>
  object(Foo)#%d (3) {
    ["bar"]=>
    int(45)
    ["prop_bar"]=>
    int(45)
    ["prop_extra"]=>
    string(19) "Not from a property"
  }
}
object(Foo)#%d (3) {
  ["bar"]=>
  int(45)
  ["prop_bar"]=>
  int(45)
  ["prop_extra"]=>
  string(19) "Not from a property"
}
