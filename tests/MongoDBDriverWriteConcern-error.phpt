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

/* HHVM-106: WriteConcern constructor should throw for invalid $w values */
try {
    new MongoDB\Driver\WriteConcern(-42);
} catch(Exception $e) {
    echo $e->getMessage(), "\n";
}
/* HHVM-106: WriteConcern constructor should throw for invalid $wtimeout values */
try {
    new MongoDB\Driver\WriteConcern("majority", -42);
} catch(Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
MongoDB\Driver\WriteConcern::__construct() expects at most 4 parameters, 5 given
Expected w to be integer or string, double given
Expected w to be >= -3, -42 given
Expected wtimeout to be >= 0, -42 given
