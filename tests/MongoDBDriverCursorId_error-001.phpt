--TEST--
MongoDB\Driver\CursorId
--FILE--
<?php
$cid = new MongoDB\Driver\CursorId("876asdf");
echo $cid, "\n";
?>
--EXPECT--
876
