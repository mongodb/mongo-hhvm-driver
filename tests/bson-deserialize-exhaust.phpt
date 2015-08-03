--TEST--
BSON deserialization should consume the whole buffer
--FILE--
<?php
$bson1 = MongoDB\BSON\fromJSON('{"x": "y"}');
$bson2 = MongoDB\BSON\fromJSON('{"a": "b"}');

try {
	$value = MongoDB\BSON\toPHP($bson1 . $bson2);
} catch (MongoDB\Driver\Exception\UnexpectedValueException $e) {
	echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Reading document did not exhaust input buffer
