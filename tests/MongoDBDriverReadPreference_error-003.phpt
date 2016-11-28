--TEST--
MongoDB\Driver\ReadPreference construction (invalid maxStalenessSeconds)
--FILE--
<?php
require_once __DIR__ . '/utils.inc';

echo throws(function() {
    new MongoDB\Driver\ReadPreference(MongoDB\Driver\ReadPreference::RP_PRIMARY, null, ['maxStalenessSeconds' => 1000]);
}, 'MongoDB\Driver\Exception\InvalidArgumentException'), "\n\n";

echo throws(function() {
    new MongoDB\Driver\ReadPreference(MongoDB\Driver\ReadPreference::RP_SECONDARY, null, ['maxStalenessSeconds' => -2]);
}, 'MongoDB\Driver\Exception\InvalidArgumentException'), "\n\n";

echo throws(function() {
    new MongoDB\Driver\ReadPreference(MongoDB\Driver\ReadPreference::RP_SECONDARY, null, ['maxStalenessSeconds' => 42]);
}, 'MongoDB\Driver\Exception\InvalidArgumentException'), "\n\n";

?>
--EXPECT--
OK: Got MongoDB\Driver\Exception\InvalidArgumentException
maxStalenessSeconds may not be used with primary mode

OK: Got MongoDB\Driver\Exception\InvalidArgumentException
Expected maxStalenessSeconds to be >= 90, -2 given

OK: Got MongoDB\Driver\Exception\InvalidArgumentException
Expected maxStalenessSeconds to be >= 90, 42 given
