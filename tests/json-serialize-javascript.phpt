--TEST--
JsonSerializable: Javascript
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
{ "foo" : "function foo(bar) { return bar; }" }
{"foo":"function foo(bar) { return bar; }"}
object(stdClass)#%d (%d) {
  ["foo"]=>
  string(33) "function foo(bar) { return bar; }"
}
