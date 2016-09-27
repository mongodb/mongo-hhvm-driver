--TEST--
MongoDB\BSON\Regex argument validation
--FILE--
<?php
require_once __DIR__ . "/utils.inc";

echo throws(function() {
    new MongoDB\BSON\Regex( "reg\0exp" );
}, 'MongoDB\Driver\Exception\InvalidArgumentException'), "\n\n";

echo throws(function() {
    new MongoDB\BSON\Regex( "reg\0exp", 'i' );
}, 'MongoDB\Driver\Exception\InvalidArgumentException'), "\n\n";

echo throws(function() {
    new MongoDB\BSON\Regex( "regexp", "a\0b" );
}, 'MongoDB\Driver\Exception\InvalidArgumentException'), "\n\n";

?>
--EXPECTF--
OK: Got MongoDB\Driver\Exception\InvalidArgumentException
Pattern cannot contain null bytes

OK: Got MongoDB\Driver\Exception\InvalidArgumentException
Pattern cannot contain null bytes

OK: Got MongoDB\Driver\Exception\InvalidArgumentException
Flags cannot contain null bytes
