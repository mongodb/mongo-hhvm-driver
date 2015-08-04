--TEST--
BSON deserialization: BSON array defaults
--FILE--
<?php
$tests = [
	'[ 42, 43, 44 ]', // a top-level can't be an array, but we test for it anyway
	'{ "nested" : [ "foo", "bar", "baz" ] }',
];

foreach ( $tests as $test )
{
	echo $test, "\n";
	$bson = MongoDB\BSON\fromJSON( $test );
	var_dump( MongoDB\BSON\toPHP( $bson ) );
	echo "\n";
}
?>
--EXPECTF--
[ 42, 43, 44 ]
object(stdClass)#%d (3) {
  [0]=>
  int(42)
  [1]=>
  int(43)
  [2]=>
  int(44)
}

{ "nested" : [ "foo", "bar", "baz" ] }
object(stdClass)#%d (1) {
  ["nested"]=>
  array(3) {
    [0]=>
    string(3) "foo"
    [1]=>
    string(3) "bar"
    [2]=>
    string(3) "baz"
  }
}
