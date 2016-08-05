--TEST--
MongoDB\BSON\Javascript::__set_state() does not allow code to contain null bytes
--FILE--
<?php
require_once __DIR__ . "/utils.inc";

echo throws(function() {
    MongoDB\BSON\Javascript::__set_state(['code' => "function foo() { return '\0'; }"]);
}, 'MongoDB\Driver\Exception\InvalidArgumentException'), "\n";

?>
--EXPECT--
OK: Got MongoDB\Driver\Exception\InvalidArgumentException
Code cannot contain null bytes
