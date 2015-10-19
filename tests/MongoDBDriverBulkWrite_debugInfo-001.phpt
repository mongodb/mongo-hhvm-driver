--TEST--
MongoDB\Driver\BulkWrite::__debugInfo
--FILE--
<?php
$bw = new MongoDB\Driver\BulkWrite;
var_dump( $bw->__debugInfo()['ordered'] );
var_dump( $bw->__debugInfo()['executed'] );

$di = $bw->__debugInfo();
foreach ( [ 'database', 'collection', 'ordered', 'executed', 'server_id', 'write_concern' ] as $field )
{
	echo $field, ": ", array_key_exists( $field, $di ) ? 'available' : 'unavailable', "\n";
}

$bw = new MongoDB\Driver\BulkWrite( [ 'ordered' => true ] );
var_dump( $bw->__debugInfo()['ordered'] );

$bw = new MongoDB\Driver\BulkWrite( [ 'ordered' => false ] );
var_dump( $bw->__debugInfo()['ordered'] );
?>
--EXPECT--
bool(true)
bool(false)
database: available
collection: available
ordered: available
executed: available
server_id: available
write_concern: available
bool(true)
bool(false)
