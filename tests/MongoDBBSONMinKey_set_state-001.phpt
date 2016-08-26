--TEST--
MongoDB\BSON\MinKey::__set_state()
--FILE--
<?php
var_export(MongoDB\BSON\MinKey::__set_state([]));
echo "\n";

?>
--EXPECT--
MongoDB\BSON\MinKey::__set_state(array(
))
