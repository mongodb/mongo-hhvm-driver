--TEST--
BSON serialization: PHP standard object to BSON document
--FILE--
<?php
class LiquidClass
{
	public $milk = "tasty";
	public $beer = "tastier";
}

class SolidClass
{
	public $bread = "okay";
	protected $cheese = "great";
	private $cant = "touch that";
}

$tests = [
	new LiquidClass,
	new SolidClass,
];

foreach ( $tests as $test )
{
	$bson = MongoDB\BSON\fromPHP( $test );
	echo MongoDB\BSON\toJSON( $bson ), "\n";
}
?>
--EXPECT--
{ "milk" : "tasty", "beer" : "tastier" }
{ "bread" : "okay" }
