--TEST--
JsonSerializable: MaxKey
--FILE--
<?php
$doc = [
	'foo' => new MongoDB\BSON\MaxKey()
];

echo MongoDB\BSON\toJSON( \MongoDB\BSON\fromPHP( $doc ) ), "\n";
$d = json_encode( $doc );
echo $d, "\n";

var_dump( \MongoDB\BSON\toPHP( \MongoDB\BSON\fromJSON( $d ) ) );
?>
--EXPECTF--
{ "foo" : { "$maxKey" : 1 } }
{"foo":{"$maxKey":1}}
object(stdClass)#%d (%d) {
  ["foo"]=>
  object(MongoDB\BSON\MaxKey)#%d (%d) {
  }
}
