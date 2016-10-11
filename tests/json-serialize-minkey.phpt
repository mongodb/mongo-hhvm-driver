--TEST--
JsonSerializable: MinKey
--FILE--
<?php
$doc = [
	'foo' => new MongoDB\BSON\MinKey()
];

echo MongoDB\BSON\toJSON( \MongoDB\BSON\fromPHP( $doc ) ), "\n";
echo json_encode( $doc ), "\n";
?>
--EXPECT--
{ "foo" : { "$minKey" : 1 } }
{"foo":{"$minKey":1}}
