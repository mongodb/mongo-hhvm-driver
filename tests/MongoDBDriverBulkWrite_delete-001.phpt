--TEST--
MongoDB\Driver\BulkWrite::delete
--FILE--
<?php
$bw = new MongoDB\Driver\BulkWrite;

// delete
$bw->delete( [ 'test' => 'new' ] );

$bw->delete( [ 'test' => 'new' ], [ 'limit' => true ] );

$bw->delete( [ 'test' => 'ruby' ], [ 'limit' => false ] );

echo $bw->count(), "\n";
?>
--EXPECT--
3
