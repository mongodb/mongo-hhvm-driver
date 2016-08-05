--TEST--
MongoDB\BSON\Javascript::__set_state() expects optional scope to be array or object
--FILE--
<?php
require_once __DIR__ . "/utils.inc";

echo throws(function() {
    MongoDB\BSON\Javascript::__set_state(['code' => 'function foo() {}',  'scope' => 'INVALID']);
}, 'MongoDB\Driver\Exception\InvalidArgumentException'), "\n";

?>
--EXPECT--
OK: Got MongoDB\Driver\Exception\InvalidArgumentException
Expected scope to be array or object, string given
