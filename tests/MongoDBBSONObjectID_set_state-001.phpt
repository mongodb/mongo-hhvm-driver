--TEST--
MongoDB\BSON\ObjectID::__set_state()
--FILE--
<?php
var_export(MongoDB\BSON\ObjectID::__set_state([
   'oid' => '576c25db6118fd406e6e6471',
]));
echo "\n";

?>
--EXPECT--
MongoDB\BSON\ObjectID::__set_state(array(
  'oid' => '576c25db6118fd406e6e6471',
))
