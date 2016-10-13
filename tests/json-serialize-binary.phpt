--TEST--
JsonSerializable: Binary
--FILE--
<?php
$doc = [
	'foo' => new MongoDB\BSON\Binary( "gargleblaster", 24 )
];

echo MongoDB\BSON\toJSON( \MongoDB\BSON\fromPHP( $doc ) ), "\n";
$d = json_encode( $doc );
echo $d, "\n";

var_dump( \MongoDB\BSON\toPHP( \MongoDB\BSON\fromJSON( $d ) ) );
?>
--EXPECTF--
{ "foo" : { "$binary" : "Z2FyZ2xlYmxhc3Rlcg==", "$type" : "18" } }
{"foo":{"$binary":"Z2FyZ2xlYmxhc3Rlcg==","$type":"18"}}
object(stdClass)#%d (%d) {
  ["foo"]=>
  object(MongoDB\BSON\Binary)#%d (%d) {
    ["data"]=>
    string(13) "gargleblaster"
    ["type"]=>
    int(24)
  }
}
