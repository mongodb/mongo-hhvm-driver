--TEST--
MongoDB\Driver\BulkWrite::update
--FILE--
<?php
$bw = new MongoDB\Driver\BulkWrite;

// replace
$bw->update( [ 'test' => 'php' ], [ 'test' => 'ruby' ] );

// update
$bw->update( [ 'test' => 'php' ], [ '$set' => [ 'test' => 'ruby' ] ] );

// multi update
$bw->update( [ 'test' => 'php' ], [ '$set' => [ 'test' => 'ruby' ] ], [ 'multi' => true ] );

// upsert
$bw->update( [ 'test' => 'new' ], [ '$set' => [ 'test' => 'ruby' ] ], [ 'upsert' => true ] );

echo $bw->count(), "\n";
?>
--EXPECT--
4
