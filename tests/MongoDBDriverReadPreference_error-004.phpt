--TEST--
MongoDB\Driver\ReadPreference construction (invalid maxStalenessMS range)
--FILE--
<?php
require_once __DIR__ . '/utils.inc';

echo throws(function() {
    new MongoDB\Driver\ReadPreference(MongoDB\Driver\ReadPreference::RP_SECONDARY, null, ['maxStalenessMS' => 2147483648]);
}, 'MongoDB\Driver\Exception\InvalidArgumentException'), "\n";

?>
--EXPECT--
OK: Got MongoDB\Driver\Exception\InvalidArgumentException
Expected maxStalenessMS to be <= 2147483647, 2147483648 given
