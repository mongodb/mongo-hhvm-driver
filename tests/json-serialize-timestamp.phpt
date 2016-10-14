--TEST--
JsonSerializable: Timestamp
--FILE--
<?php
$doc = [
	'foo' => new MongoDB\BSON\Timestamp( "1234", "5678" )
];

echo MongoDB\BSON\toJSON( \MongoDB\BSON\fromPHP( $doc ) ), "\n";
$d = json_encode( $doc );
echo $d, "\n";

var_dump( \MongoDB\BSON\toPHP( \MongoDB\BSON\fromJSON( $d ) ) );
?>
--EXPECTF--
{ "foo" : { "$timestamp" : { "t" : 5678, "i" : 1234 } } }
{"foo":{"$timestamp":{"t":5678,"i":1234}}}
object(stdClass)#%d (%d) {
  ["foo"]=>
  object(MongoDB\BSON\Timestamp)#%d (%d) {
    ["increment"]=>
    string(%d) "1234"
    ["timestamp"]=>
    string(%d) "5678"
  }
}
