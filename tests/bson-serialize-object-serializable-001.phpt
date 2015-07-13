--TEST--
BSON serialization: PHP serializable object to BSON document
--FILE--
<?php
class LiquidClass implements MongoDB\BSON\Serializable
{
	public $type;

	function __construct( $type )
	{
		$this->type = $type;
	}

	function bsonSerialize()
	{
		return [ 'type' => $this->type ];
	}
}

$tests = [
	new LiquidClass( "beer" ),
	new LiquidClass( [ "beer", "wine" ] ),
	new LiquidClass( new LiquidClass( "whisky" ) ),
];

foreach ( $tests as $test )
{
	$bson = MongoDB\BSON\fromPHP( $test );
	echo MongoDB\BSON\toJSON( $bson ), "\n";
}
?>
--EXPECT--
{ "type" : "beer" }
{ "type" : [ "beer", "wine" ] }
{ "type" : { "type" : "whisky" } }
