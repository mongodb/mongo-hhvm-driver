--TEST--
BSON serialization: PHP persistable object write overwritten __pclass
--SKIPIF--
<?php if (!defined("HHVM_VERSION_ID")) exit("skip PHPC encodes __pclass first"); ?>
--FILE--
<?php
class LiquidClass implements MongoDB\BSON\Persistable
{
	public $type;
	public $pclassValue;

	function __construct( $type, $pclassValue )
	{
		$this->type = $type;
		$this->pclassValue = $pclassValue;
	}

	function bsonSerialize()
	{
		return [ 'type' => $this->type, '__pclass' => $this->pclassValue ];
	}

	function bsonUnserialize( array $elements )
	{
	}
}

$tests = [
	new LiquidClass( "beer", 42 ),
	new LiquidClass( "beer", new MongoDB\BSON\Binary( "FooClass", 0x44 ) ),
];

foreach ( $tests as $test )
{
	$bson = MongoDB\BSON\fromPHP( $test );
	echo MongoDB\BSON\toJSON( $bson ), "\n";
}
?>
--EXPECT--
{ "type" : "beer", "__pclass" : { "$type" : "80", "$binary" : "TGlxdWlkQ2xhc3M=" } }
{ "type" : "beer", "__pclass" : { "$type" : "80", "$binary" : "TGlxdWlkQ2xhc3M=" } }
