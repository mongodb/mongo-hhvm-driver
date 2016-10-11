--TEST--
JsonSerializable: Regex
--FILE--
<?php
$doc = [
	'foo' => new MongoDB\BSON\Regex( "/foo/", "i" )
];

echo MongoDB\BSON\toJSON( \MongoDB\BSON\fromPHP( $doc ) ), "\n";
echo json_encode( $doc ), "\n";
?>
--EXPECT--
{ "foo" : { "$regex" : "/foo/", "$options" : "i" } }
{"foo":{"$regex":"\/foo\/","$options":"i"}}
