--TEST--
JsonSerializable: Regex without flags
--FILE--
<?php
$doc = [
	'foo' => new MongoDB\BSON\Regex( "/foo/" )
];

echo MongoDB\BSON\toJSON( \MongoDB\BSON\fromPHP( $doc ) ), "\n";
$d = json_encode( $doc );
echo $d, "\n";

var_dump( \MongoDB\BSON\toPHP( \MongoDB\BSON\fromJSON( $d ) ) );
?>
--EXPECTF--
{ "foo" : { "$regex" : "/foo/", "$options" : "" } }
{"foo":{"$regex":"\/foo\/","$options":""}}
object(stdClass)#%d (%d) {
  ["foo"]=>
  object(MongoDB\BSON\Regex)#%d (%d) {
    ["pattern"]=>
    string(%d) "/foo/"
    ["flags"]=>
    string(%d) ""
  }
}
