--TEST--
MongoDB\Driver\Manager::__construct(): invalid read preference
--FILE--
<?php
include 'utils.inc';

echo throws(function() {
    $manager = new MongoDB\Driver\Manager( 'mongodb://localhost/?readPreference=primary&readPreferenceTags=dc:ny');
}, "MongoDB\Driver\Exception\InvalidArgumentException"), "\n";

echo throws(function() {
    $manager = new MongoDB\Driver\Manager( 'mongodb://localhost', [ 'readPreference' => 'nothing' ] );
}, "MongoDB\Driver\Exception\InvalidArgumentException"), "\n";

echo throws(function() {
    $manager = new MongoDB\Driver\Manager( 'mongodb://localhost/?readPreference=primary', [ 'readPreferenceTags' => [ [ ] ] ] );
}, "MongoDB\Driver\Exception\InvalidArgumentException"), "\n";

echo throws(function() {
    $manager = new MongoDB\Driver\Manager( 'mongodb://localhost/?readPreference=primary', [ 'readPreferenceTags' => [ 'invalid' ] ] );
}, "MongoDB\Driver\Exception\InvalidArgumentException"), "\n";

echo throws(function() {
    $manager = new MongoDB\Driver\Manager( 'mongodb://localhost/?readPreference=secondary', [ 'readPreferenceTags' => [ 'invalid' ] ] );
}, "MongoDB\Driver\Exception\InvalidArgumentException"), "\n";

?>
--EXPECT--
OK: Got MongoDB\Driver\Exception\InvalidArgumentException
Failed to parse MongoDB URI: 'mongodb://localhost/?readPreference=primary&readPreferenceTags=dc:ny'
OK: Got MongoDB\Driver\Exception\InvalidArgumentException
Unsupported readPreference value: 'nothing'
OK: Got MongoDB\Driver\Exception\InvalidArgumentException
Primary read preference mode conflicts with tags
OK: Got MongoDB\Driver\Exception\InvalidArgumentException
Read preference tags must be an array of zero or more documents
OK: Got MongoDB\Driver\Exception\InvalidArgumentException
Read preference tags must be an array of zero or more documents
