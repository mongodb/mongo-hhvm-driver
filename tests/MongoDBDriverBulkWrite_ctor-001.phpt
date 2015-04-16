--TEST--
MongoDB\Driver\BulkWrite
--FILE--
<?php
$bw = new MongoDB\Driver\BulkWrite;
var_dump( $bw->__debugInfo()['ordered'] );

$bw = new MongoDB\Driver\BulkWrite( false );
var_dump( $bw->__debugInfo()['ordered'] );

$bw = new MongoDB\Driver\BulkWrite( true );
var_dump( $bw->__debugInfo()['ordered'] );
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
