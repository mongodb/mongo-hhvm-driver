--TEST--
MongoDB\BSON\MaxKey::__set_state()
--FILE--
<?php
var_export(MongoDB\BSON\MaxKey::__set_state([]));
echo "\n";

?>
--EXPECT--
MongoDB\BSON\MaxKey::__set_state(array(
))
