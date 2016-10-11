--TEST--
JsonSerializable: Regex without flags
--FILE--
<?php
$doc = [
	'foo' => new MongoDB\BSON\Regex( "/foo/" )
];

echo MongoDB\BSON\toJSON( \MongoDB\BSON\fromPHP( $doc ) ), "\n";
echo json_encode( $doc ), "\n";
?>
--EXPECT--
{ "foo" : { "$regex" : "/foo/", "$options" : "" } }
{"foo":{"$regex":"\/foo\/","$options":""}}
