--TEST--
JsonSerializable: UTCDateTime
--FILE--
<?php
$doc = [
	'foo' => new MongoDB\BSON\UTCDateTime( new \DateTime( '2016-10-11 13:34:26 UTC' ) )
];

echo MongoDB\BSON\toJSON( \MongoDB\BSON\fromPHP( $doc ) ), "\n";
$d = json_encode( $doc );
echo $d, "\n";

var_dump( \MongoDB\BSON\toPHP( \MongoDB\BSON\fromJSON( $d ) ) );
?>
--EXPECTF--
{ "foo" : { "$date" : 1476192866000 } }
{"foo":{"$date":1476192866000}}
object(stdClass)#%d (%d) {
  ["foo"]=>
  object(MongoDB\BSON\UTCDateTime)#%d (%d) {
    ["milliseconds"]=>
    string(13) "1476192866000"
  }
}
