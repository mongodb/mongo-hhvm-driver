--TEST--
MongoDB\BSON\Binary::getType()
--FILE--
<?php
$a = new MongoDB\BSON\Binary("Hello!", 0x00);
$b = new MongoDB\BSON\Binary("Hello!", 0x44);

var_dump($a->getType(), $b->getType());
?>
--EXPECTF--
int(0)
int(68)
