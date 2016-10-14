--TEST--
JsonSerializable: MinKey
--FILE--
<?php
$doc = [
	'foo' => new MongoDB\BSON\MinKey()
];

echo MongoDB\BSON\toJSON( \MongoDB\BSON\fromPHP( $doc ) ), "\n";
$d = json_encode( $doc );
echo $d, "\n";

var_dump( \MongoDB\BSON\toPHP( \MongoDB\BSON\fromJSON( $d ) ) );
?>
--EXPECTF--
{ "foo" : { "$minKey" : 1 } }
{"foo":{"$minKey":1}}
object(stdClass)#%d (%d) {
  ["foo"]=>
  object(MongoDB\BSON\MinKey)#%d (%d) {
  }
}
