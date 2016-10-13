--TEST--
JsonSerializable: Decimal128
--FILE--
<?php
$doc = [
	'foo' => new MongoDB\BSON\Decimal128( "12389719287312" )
];

echo MongoDB\BSON\toJSON( \MongoDB\BSON\fromPHP( $doc ) ), "\n";
$d = json_encode( $doc );
echo $d, "\n";

var_dump( \MongoDB\BSON\toPHP( \MongoDB\BSON\fromJSON( $d ) ) );
?>
--EXPECTF--
{ "foo" : { "$numberDecimal" : "12389719287312" } }
{"foo":{"$numberDecimal":"12389719287312"}}
object(stdClass)#%d (%d) {
  ["foo"]=>
  object(MongoDB\BSON\Decimal128)#%d (%d) {
    ["dec"]=>
    string(14) "12389719287312"
  }
}
