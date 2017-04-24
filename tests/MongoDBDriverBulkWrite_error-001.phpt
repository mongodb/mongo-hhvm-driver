--TEST--
MongoDB\Driver\Manager::executeBulkWrite() duplicate key error message
--FILE--
<?php
define( 'NS', 'demo.test' );
$manager = new MongoDB\Driver\Manager();

$c = new MongoDB\Driver\Command( [ 'drop' => 'test'] );
try {
	$manager->executeCommand( 'demo', $c );
}
catch ( MongoDB\Driver\Exception\RuntimeException $e )
{
	// Ignore "ns not found" errors
	if ( $e->getCode() == 59 ) {
		throw $e;
	}
}
$bulk = new MongoDB\Driver\BulkWrite();
$bulk->insert(array('_id' => 1, 'x' => 1));
$bulk->insert(array('_id' => 1, 'x' => 1));
try {
    $manager->executeBulkWrite(NS, $bulk);
} catch (\MongoDB\Driver\Exception\BulkWriteException $e) {
    printf("BulkWriteException: %s\n", $e->getMessage());
}
?>
--EXPECTF--
BulkWriteException: E11000 duplicate key error %s_id_ dup key: { : 1 }
