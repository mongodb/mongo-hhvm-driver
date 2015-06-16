--TEST--
MongoDB\Driver\WriteResult::__construct: private constructor
--INI--
xdebug.enable=0
--FILE--
<?php
$w = new MongoDB\Driver\WriteResult();
?>
--EXPECTF--
%s: Call to private MongoDB\Driver\WriteResult::__construct() from invalid context %s
