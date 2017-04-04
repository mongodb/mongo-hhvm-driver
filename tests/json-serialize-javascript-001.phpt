--TEST--
JsonSerializable: Javascript (with scope)
--FILE--
<?php
$doc = [
	'foo' => new MongoDB\BSON\Javascript( "function foo(bar) { return bar; }", [ 'foo' => 42 ] )
];

echo MongoDB\BSON\toJSON( \MongoDB\BSON\fromPHP( $doc ) ), "\n";
$d = json_encode( $doc );
echo $d, "\n";

var_dump( \MongoDB\BSON\toPHP( \MongoDB\BSON\fromJSON( $d ) ) );
?>
--EXPECTF--
{ "foo" : { "$code" : "function foo(bar) { return bar; }", "$scope" : { "foo" : 42 } } }
{"foo":{"$code":"function foo(bar) { return bar; }","$scope":{"foo":42}}}
object(stdClass)#%d (%d) {
  ["foo"]=>
  object(MongoDB\BSON\Javascript)#%d (%d) {
    ["code"]=>
    string(33) "function foo(bar) { return bar; }"
    ["scope"]=>
    object(stdClass)#%d (%d) {
      ["foo"]=>
      int(42)
    }
  }
}
