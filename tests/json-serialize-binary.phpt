--TEST--
JsonSerializable: Binary
--FILE--
<?php
$doc = [
	'foo' => new MongoDB\BSON\Binary( "gargleblaster", 24 )
];

echo MongoDB\BSON\toJSON( \MongoDB\BSON\fromPHP( $doc ) ), "\n";
echo json_encode( $doc ), "\n";
?>
--EXPECT--
{ "foo" : { "$binary" : "Z2FyZ2xlYmxhc3Rlcg==", "$type" : "18" } }
{"foo":{"$binary":"Z2FyZ2xlYmxhc3Rlcg==","$type":"18"}}
