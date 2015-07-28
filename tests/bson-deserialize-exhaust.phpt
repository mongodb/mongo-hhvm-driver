--TEST--
BSON deserialization should consume the whole buffer
--FILE--
<?php
$bson1 = MongoDB\BSON\fromJSON('{"x": "y"}');
$bson2 = MongoDB\BSON\fromJSON('{"a": "b"}');
$value = MongoDB\BSON\toPHP($bson1 . $bson2);

var_dump($value);
?>
--EXPECTF--
Warning:%sReading document did not exhaust input buffer in %s on line %d
NULL
