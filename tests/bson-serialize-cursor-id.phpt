--TEST--
MongoDB\Driver\CursorID serialization
--FILE--
<?php
include dirname(__FILE__) . '/utils.inc';

$m = new MongoDB\Driver\Manager("mongodb://localhost:27017");
cleanup( $m );

$bw = new MongoDB\Driver\BulkWrite( [ 'ordered' => false ] );
$bw->insert( [ 'test' => 'foo', 'i' => 1 ] );
$bw->insert( [ 'test' => 'foo', 'i' => 2 ] );
$bw->insert( [ 'test' => 'foo', 'i' => 3 ] );
$result = $m->executeBulkWrite( 'demo.test', $bw );

$q = new MongoDB\Driver\Query( [], [ 'batchSize' => 2 ] );
$cursorId = $m->executeQuery( 'demo.test', $q )->getId();

echo "result:\n";
hex_dump( MongoDB\BSON\fromPHP( [ 'cid' => $cursorId ] ) );
?>
--EXPECTF--
result:
     0 : 12 00 00 00 12 63 69 64 00 %x %x %x %x %x %x %x  [.....cid.%s]
    10 : %x 00                                            [%s.]
