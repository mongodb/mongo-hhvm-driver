--TEST--
MongoDB\Driver\WriteConcern construction (invalid arguments)
--SKIPIF--
<?php if (!defined("HHVM_VERSION_ID")) exit("skip PHP throws InvalidArgumentException instead of Exception"); ?>
--FILE--
<?php

try {
    new MongoDB\Driver\WriteConcern("string", 10000, false, true, 1);
} catch(Exception $e) {
    echo $e->getMessage(), "\n";
}

/* HHVM-105: WriteConcern constructor should throw if {{$w}} argument is not an integer or string */
try {
    new MongoDB\Driver\WriteConcern(M_PI);
} catch(Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
MongoDB\Driver\WriteConcern::__construct() expects at most 4 parameters, 5 given
Expected w to be integer or string, double given
