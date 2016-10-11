--TEST--
JsonSerializable: Javascript
--FILE--
<?php
$doc = [
	'foo' => new MongoDB\BSON\Javascript( "function foo(bar) { return bar; }", [ 'foo' => 42 ] )
];

echo MongoDB\BSON\toJSON( \MongoDB\BSON\fromPHP( $doc ) ), "\n";
echo json_encode( $doc ), "\n";
?>
--EXPECT--
{ "foo" : "function foo(bar) { return bar; }" }
{"foo":"function foo(bar) { return bar; }"}
