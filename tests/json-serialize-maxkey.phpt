--TEST--
JsonSerializable: MaxKey
--FILE--
<?php
$doc = [
	'foo' => new MongoDB\BSON\MaxKey()
];

echo MongoDB\BSON\toJSON( \MongoDB\BSON\fromPHP( $doc ) ), "\n";
echo json_encode( $doc ), "\n";
?>
--EXPECT--
{ "foo" : { "$maxKey" : 1 } }
{"foo":{"$maxKey":1}}
