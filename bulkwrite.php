<?php
$bw = new MongoDB\Driver\BulkWrite(true);

//insert
echo $bw->insert( [ 'test' => 'foo' ] ), "\n";

// replace
$bw->update( [ 'test' => 'php' ], [ 'test' => 'ruby' ] );

// update
$bw->update( [ 'test' => 'php' ], [ '$set' => [ 'test' => 'ruby' ] ] );

// multi update
$bw->update( [ 'test' => 'php' ], [ '$set' => [ 'test' => 'ruby' ] ], [ 'multi' => true ] );

// upsert
$bw->update( [ 'test' => 'new' ], [ '$set' => [ 'test' => 'ruby' ] ], [ 'upsert' => true ] );

// delete
$bw->delete( [ 'test' => 'new' ] );
$bw->delete( [ 'test' => 'new' ], [ 'limit' => true ] );

$bw->delete( [ 'test' => 'ruby' ], [ 'limit' => false ] );

echo $bw->count(), "\n";

?>
