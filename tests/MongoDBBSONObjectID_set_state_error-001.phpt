--TEST--
MongoDB\BSON\ObjectID::__set_state() requires "oid" string field
--FILE--
<?php
require_once __DIR__ . "/utils.inc";

echo throws(function() {
    MongoDB\BSON\ObjectID::__set_state(['oid' => 0]);
}, 'MongoDB\Driver\Exception\InvalidArgumentException'), "\n";

?>
--EXPECT--
OK: Got MongoDB\Driver\Exception\InvalidArgumentException
MongoDB\BSON\ObjectID initialization requires "oid" string field
