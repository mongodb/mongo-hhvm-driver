--TEST--
MongoDB\Driver\Manager: maxStalenessSeconds
--FILE--
<?php
// These all should work, and produce no output. We're not testing whether the flag actually does something
$manager = new MongoDB\Driver\Manager("mongodb://localhost/?readPreference=SECONDARY&maxStalenessSeconds=-1");
$manager = new MongoDB\Driver\Manager("mongodb://localhost/?readPreference=SECONDARY", [ 'maxStalenessSeconds' => -1 ] );

$manager = new MongoDB\Driver\Manager("mongodb://localhost/?readPreference=SECONDARY&maxstalenessseconds=-1");
$manager = new MongoDB\Driver\Manager("mongodb://localhost/?readPreference=SECONDARY", [ 'maxstalenessseconds' => -1 ] );

$manager = new MongoDB\Driver\Manager("mongodb://localhost/?readPreference=SECONDARY&maxStalenessSeconds=1231");
$manager = new MongoDB\Driver\Manager("mongodb://localhost/?readPreference=SECONDARY", [ 'maxStalenessSeconds' => 1231 ] );

$manager = new MongoDB\Driver\Manager("mongodb://localhost/?readPreference=SECONDARY&maxstalenessseconds=1231");
$manager = new MongoDB\Driver\Manager("mongodb://localhost/?readPreference=SECONDARY", [ 'maxstalenessseconds' => 1231 ] );
?>
==DONE==
--EXPECTF--
==DONE==
