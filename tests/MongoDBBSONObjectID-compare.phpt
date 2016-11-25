--TEST--
MongoDB\BSON\ObjectID comparison
--FILE--
<?php

$id = new MongoDB\BSON\ObjectID("53e2a1c40640fd72175d4603");
$test = new stdClass;
$test->my = $id;

$s = \MongoDB\BSON\fromPHP( $test );
$json = \MongoDB\BSON\toJSON( $s );

$bson = \MongoDB\BSON\fromJSON( $json );
$testagain = \MongoDB\BSON\toPHP( $bson );

var_dump( \MongoDB\BSON\toJSON( \MongoDB\BSON\fromPHP( $test ) ), \MongoDB\BSON\toJSON( \MongoDB\BSON\fromPHP( $testagain ) ) );
var_dump( (object)$test == (object)$testagain );
?>
--EXPECTF--
string(50) "{ "my" : { "$oid" : "53e2a1c40640fd72175d4603" } }"
string(50) "{ "my" : { "$oid" : "53e2a1c40640fd72175d4603" } }"
bool(true)
