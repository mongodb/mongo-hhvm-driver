--TEST--
JsonSerializable: ObjectID
--FILE--
<?php
$doc = [
	'foo' => new MongoDB\BSON\ObjectID()
];

echo MongoDB\BSON\toJSON( \MongoDB\BSON\fromPHP( $doc ) ), "\n";
$d = json_encode( $doc );
echo $d, "\n";

var_dump( \MongoDB\BSON\toPHP( \MongoDB\BSON\fromJSON( $d ) ) );
?>
--EXPECTF--
{ "foo" : { "$oid" : "%s" } }
{"foo":{"$oid":"%s"}}
object(stdClass)#%d (%d) {
  ["foo"]=>
  object(MongoDB\BSON\ObjectID)#%d (%d) {
    ["oid"]=>
    string(24) "%s"
  }
}
