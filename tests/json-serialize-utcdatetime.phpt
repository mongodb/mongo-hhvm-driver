--TEST--
JsonSerializable: UTCDateTime
--FILE--
<?php
$doc = [
	'foo' => new MongoDB\BSON\UTCDateTime( new \DateTime( '2016-10-11 13:34:26' ) )
];

echo MongoDB\BSON\toJSON( \MongoDB\BSON\fromPHP( $doc ) ), "\n";
echo json_encode( $doc ), "\n";
?>
--EXPECT--
{ "foo" : { "$date" : 1476189266000 } }
{"foo":{"$date":1476189266000}}
