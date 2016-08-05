--TEST--
MongoDB\BSON\Javascript::__set_state() requires "code" string field
--FILE--
<?php
require_once __DIR__ . "/utils.inc";

echo throws(function() {
    MongoDB\BSON\Javascript::__set_state(['code' => 0]);
}, 'MongoDB\Driver\Exception\InvalidArgumentException'), "\n";

?>
--EXPECT--
OK: Got MongoDB\Driver\Exception\InvalidArgumentException
MongoDB\BSON\Javascript initialization requires "code" string field
