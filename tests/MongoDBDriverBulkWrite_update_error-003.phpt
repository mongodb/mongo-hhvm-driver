--TEST--
MongoDB\Driver\BulkWrite::update() with invalid options
--FILE--
<?php

require_once __DIR__ . '/utils.inc';

$bulk = new MongoDB\Driver\BulkWrite;

echo throws(function() use ($bulk) {
    $bulk->update(['x' => 1], ['y' => 1], ['multi' => true]);
}, 'MongoDB\Driver\Exception\InvalidArgumentException'), "\n\n";

echo throws(function() use ($bulk) {
    $bulk->update(['x' => 1], ['y' => 1], ['collation' => 1]);
}, 'MongoDB\Driver\Exception\InvalidArgumentException'), "\n\n";

echo throws(function() use ($bulk) {
    $bulk->update(['x' => 1], ['$set' => ['y' => 1]], ['collation' => 1]);
}, 'MongoDB\Driver\Exception\InvalidArgumentException'), "\n";

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
OK: Got MongoDB\Driver\Exception\InvalidArgumentException
Replacement document conflicts with true "multi" option

OK: Got MongoDB\Driver\Exception\InvalidArgumentException
Expected "collation" option to be array or object, integer given

OK: Got MongoDB\Driver\Exception\InvalidArgumentException
Expected "collation" option to be array or object, integer given
===DONE===
