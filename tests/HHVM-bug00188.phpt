--TEST--
Test for HHVM-181: ObjectID only supports lower case hexadecimal letters
--FILE--
<?php
$a = new \MongoDB\BSON\ObjectID('56925B7330616224D0000001');
$b = new \MongoDB\BSON\ObjectID('56925b7330616224d0000001');

var_dump( $a, $b );
?>
--EXPECTF--
object(MongoDB\BSON\ObjectID)#%d (1) {
  ["oid"]=>
  string(24) "56925b7330616224d0000001"
}
object(MongoDB\BSON\ObjectID)#%d (1) {
  ["oid"]=>
  string(24) "56925b7330616224d0000001"
}
