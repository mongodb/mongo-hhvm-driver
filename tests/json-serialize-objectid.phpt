--TEST--
JsonSerializable: ObjectID
--FILE--
<?php
$doc = [
	'foo' => new MongoDB\BSON\ObjectID()
];

echo MongoDB\BSON\toJSON( \MongoDB\BSON\fromPHP( $doc ) ), "\n";
echo json_encode( $doc ), "\n";
?>
--EXPECTF--
{ "foo" : { "$oid" : "%s" } }
{"foo":{"$oid":"%s"}}

