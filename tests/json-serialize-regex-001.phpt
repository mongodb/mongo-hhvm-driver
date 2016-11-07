--TEST--
JsonSerializable: Regex
--FILE--
<?php
$doc = [
	'foo' => new MongoDB\BSON\Regex( "pattern", "i" )
];

echo MongoDB\BSON\toJSON( \MongoDB\BSON\fromPHP( $doc ) ), "\n";
$d = json_encode( $doc );
echo $d, "\n";

var_dump( \MongoDB\BSON\toPHP( \MongoDB\BSON\fromJSON( $d ) ) );
?>
--EXPECTF--
{ "foo" : { "$regex" : "pattern", "$options" : "i" } }
{"foo":{"$regex":"pattern","$options":"i"}}
object(stdClass)#%d (%d) {
  ["foo"]=>
  object(MongoDB\BSON\Regex)#%d (%d) {
    ["pattern"]=>
    string(%d) "pattern"
    ["flags"]=>
    string(%d) "i"
  }
}
