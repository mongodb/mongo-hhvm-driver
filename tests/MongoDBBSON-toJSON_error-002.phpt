--TEST--
MongoDB\BSON\toJSON(): BSON decoding exceptions for malformed documents
--FILE--
<?php
include 'utils.inc';

$tests = array(
    pack('Vx', 4), // Empty document with invalid length (too small)
    pack('Vx', 6), // Empty document with invalid length (too large)
);

foreach ($tests as $bson) {
    echo throws(function() use ($bson) {
        MongoDB\BSON\toJSON($bson);
    }, 'MongoDB\Driver\Exception\UnexpectedValueException'), "\n";
}

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
OK: Got MongoDB\Driver\Exception\UnexpectedValueException
Could not read document from BSON reader
OK: Got MongoDB\Driver\Exception\UnexpectedValueException
Could not read document from BSON reader
===DONE===
