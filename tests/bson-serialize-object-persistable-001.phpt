--TEST--
BSON serialization: PHP persistable object
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
{ "type" : "beer", "__pclass" : { "$type" : "80", "$binary" : "TGlxdWlkQ2xhc3M=" } }
{ "type" : [ "beer", "wine" ], "__pclass" : { "$type" : "80", "$binary" : "TGlxdWlkQ2xhc3M=" } }
{ "type" : { "type" : "whisky", "__pclass" : { "$type" : "80", "$binary" : "TGlxdWlkQ2xhc3M=" } }, "__pclass" : { "$type" : "80", "$binary" : "TGlxdWlkQ2xhc3M=" } }
