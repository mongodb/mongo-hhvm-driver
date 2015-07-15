--TEST--
BSON deserialization: BSON root defaults
--FILE--
<?php
$tests = [
	'{ "foo" : 42 }',
	'{ "foo" : "yes", "__pclass": "MyObject" }',
	'{ "foo" : "yes", "__pclass": { "$type": "44", "$binary" : "TGlxdWlkQ2xhc3M=" } }',
];

foreach ( $tests as $test )
{
	echo $test, "\n";
	$bson = MongoDB\BSON\fromJSON( $test );
	var_dump( MongoDB\BSON\toPHP( $bson ) );
	echo "\n";
}
?>
--EXPECT--
{ "foo" : 42 }
object(stdClass)#1 (1) {
  ["foo"]=>
  int(42)
}

{ "foo" : "yes", "__pclass": "MyObject" }
object(stdClass)#1 (2) {
  ["foo"]=>
  string(3) "yes"
  ["__pclass"]=>
  string(8) "MyObject"
}

{ "foo" : "yes", "__pclass": { "$type": "44", "$binary" : "TGlxdWlkQ2xhc3M=" } }
object(stdClass)#2 (2) {
  ["foo"]=>
  string(3) "yes"
  ["__pclass"]=>
  object(MongoDB\BSON\Binary)#1 (2) {
    ["data":"MongoDB\BSON\Binary":private]=>
    string(11) "LiquidClass"
    ["subType":"MongoDB\BSON\Binary":private]=>
    int(68)
  }
}
