--TEST--
MongoDB\Driver\Manager::executeDelete
--FILE--
<?php
$m = new MongoDB\Driver\Manager("mongodb://localhost:27017");

$c = new MongoDB\Driver\Command( [ 'drop' => 'test'] );
$cursor = $m->executeCommand( 'demo', $c );

/* Inserting some data */
$m->executeInsert( 'demo.test', [ 'd' => 2 ] );
$m->executeInsert( 'demo.test', [ 'd' => 3 ] );
$m->executeInsert( 'demo.test', [ 'd' => 4 ] );
$m->executeInsert( 'demo.test', [ 'd' => 5 ] );
$m->executeInsert( 'demo.test', [ 'd' => 6 ] );
$m->executeInsert( 'demo.test', [ 'd' => 7 ] );
$m->executeInsert( 'demo.test', [ 'd' => 8 ] );
$m->executeInsert( 'demo.test', [ 'd' => 9 ] );

// delete one
$m->executeDelete( 'demo.test', [ 'd' => 2 ], [ 'limit' => true ] );

// multi delete
$m->executeDelete( 'demo.test', [ 'd' => [ '$gte' => 4, '$lte' => 5 ] ], [ 'limit' => false ] );
$m->executeDelete( 'demo.test', [ 'd' => [ '$gte' => 7, '$lte' => 8 ] ] );

var_dump( $m->executeQuery( 'demo.test', new MongoDB\Driver\Query( [] ) )->toArray() );
?>
--EXPECTF--
array(3) {
  [0]=>
  object(stdClass)#7 (2) {
    ["_id"]=>
    object(MongoDB\BSON\ObjectId)#6 (0) {
    }
    ["d"]=>
    int(3)
  }
  [1]=>
  object(stdClass)#9 (2) {
    ["_id"]=>
    object(MongoDB\BSON\ObjectId)#8 (0) {
    }
    ["d"]=>
    int(6)
  }
  [2]=>
  object(stdClass)#11 (2) {
    ["_id"]=>
    object(MongoDB\BSON\ObjectId)#10 (0) {
    }
    ["d"]=>
    int(9)
  }
}
