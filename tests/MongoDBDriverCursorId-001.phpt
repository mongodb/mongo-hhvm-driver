--TEST--
MongoDB\Driver\CursorId
--FILE--
<?php
$cid = new MongoDB\Driver\CursorId("0123123");
echo $cid, "\n";
?>
--EXPECT--
123123
