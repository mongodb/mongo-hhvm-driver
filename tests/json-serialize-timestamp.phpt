--TEST--
JsonSerializable: Timestamp
--FILE--
<?php
$doc = [
	'foo' => new MongoDB\BSON\Timestamp( "1234", "5678" )
];

echo MongoDB\BSON\toJSON( \MongoDB\BSON\fromPHP( $doc ) ), "\n";
echo json_encode( $doc ), "\n";
?>
--EXPECT--
{ "foo" : { "$timestamp" : { "t" : 5678, "i" : 1234 } } }
{"foo":{"$timestamp":{"t":5678,"i":1234}}}
