--TEST--
MongoDB\BSON\Binary::getData()
--FILE--
<?php
$a = new MongoDB\BSON\Binary("Hello!", 0x00);
$b = new MongoDB\BSON\Binary("Hello!", 0x44);

var_dump($a->getData(), $b->getData());
?>
--EXPECTF--
string(6) "Hello!"
string(6) "Hello!"
