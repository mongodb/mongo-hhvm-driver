--TEST--
JsonSerializable: Decimal128
--FILE--
<?php
$doc = [
	'foo' => new MongoDB\BSON\Decimal128( "12389719287312" )
];

echo MongoDB\BSON\toJSON( \MongoDB\BSON\fromPHP( $doc ) ), "\n";
echo json_encode( $doc ), "\n";
?>
--EXPECT--
{ "foo" : { "$numberDecimal" : "12389719287312" } }
{"foo":{"$numberDecimal":"12389719287312"}}
