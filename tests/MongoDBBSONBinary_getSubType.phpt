--TEST--
MongoDB\BSON\Binary::getSubType()
--FILE--
<?php
$a = new MongoDB\BSON\Binary("Hello!", 0x00);
$b = new MongoDB\BSON\Binary("Hello!", 0x44);

var_dump($a->getSubType(), $b->getSubType());
?>
--EXPECTF--
int(0)
int(68)
