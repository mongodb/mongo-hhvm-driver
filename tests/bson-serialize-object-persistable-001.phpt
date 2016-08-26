--TEST--
BSON serialization: PHP persistable object
--SKIPIF--
<?php if (!defined("HHVM_VERSION_ID")) exit("skip PHPC encodes __pclass first"); ?>
--FILE--
<?php
class LiquidClass implements MongoDB\BSON\Persistable
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

	function bsonUnserialize( array $elements )
	{
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
{ "type" : "beer", "__pclass" : { "$binary" : "TGlxdWlkQ2xhc3M=", "$type" : "80" } }
{ "type" : [ "beer", "wine" ], "__pclass" : { "$binary" : "TGlxdWlkQ2xhc3M=", "$type" : "80" } }
{ "type" : { "type" : "whisky", "__pclass" : { "$binary" : "TGlxdWlkQ2xhc3M=", "$type" : "80" } }, "__pclass" : { "$binary" : "TGlxdWlkQ2xhc3M=", "$type" : "80" } }
