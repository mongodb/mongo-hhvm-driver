--TEST--
MongoDB\Driver\Manager::executeUpdate
--FILE--
<?php
include 'utils.inc';

$m = new MongoDB\Driver\Manager("mongodb://localhost:27017");

$c = new MongoDB\Driver\Command( [ 'drop' => 'test'] );
$cursor = $m->executeCommand( 'demo', $c );

/* Inserting some data */
$m->executeInsert( 'demo.test', [ 'd' => 4 ] );
$m->executeInsert( 'demo.test', [ 'd' => 5 ] );
$m->executeInsert( 'demo.test', [ 'd' => 6 ] );
$m->executeInsert( 'demo.test', [ 'd' => 7 ] );
$m->executeInsert( 'demo.test', [ 'd' => 8 ] );
$m->executeInsert( 'demo.test', [ 'd' => 9 ] );

// replace
show_obj_properties( $m->executeUpdate( 'demo.test', [ 'd' => 4 ], [ 'test' => 'four' ] ), [ 'matched', 'modified', 'upserted' ] );

// update
show_obj_properties( $m->executeUpdate( 'demo.test', [ 'd' => 5 ], [ '$set' => [ 'test' => 'five' ] ] ), [ 'matched', 'modified', 'upserted' ] );

// multi update
show_obj_properties( $m->executeUpdate( 'demo.test', [ 'd' => [ '$gte' => 7 ] ], [ '$set' => [ 'test' => '>seven' ] ], [ 'multi' => true ] ), [ 'matched', 'modified', 'upserted' ] );

// upsert
show_obj_properties( $m->executeUpdate( 'demo.test', [ 'd' => 10 ], [ '$set' => [ 'test' => 'ten' ] ], [ 'upsert' => true ] ), [ 'matched', 'modified', 'upserted' ] );

var_dump( $m->executeQuery( 'demo.test', new MongoDB\Driver\Query( [] ) )->toArray() );
?>
--EXPECTF--
matched: 1; modified: 1; upserted: 0; 
matched: 1; modified: 1; upserted: 0; 
matched: 3; modified: 3; upserted: 0; 
matched: 0; modified: 0; upserted: 1; 
array(7) {
  [0]=>
  object(stdClass)#%d (2) {
    ["_id"]=>
    object(MongoDB\BSON\ObjectId)#%d (0) {
    }
    ["test"]=>
    string(4) "four"
  }
  [1]=>
  object(stdClass)#%d (3) {
    ["_id"]=>
    object(MongoDB\BSON\ObjectId)#%d (0) {
    }
    ["d"]=>
    int(5)
    ["test"]=>
    string(4) "five"
  }
  [2]=>
  object(stdClass)#%d (2) {
    ["_id"]=>
    object(MongoDB\BSON\ObjectId)#%d (0) {
    }
    ["d"]=>
    int(6)
  }
  [3]=>
  object(stdClass)#%d (3) {
    ["_id"]=>
    object(MongoDB\BSON\ObjectId)#%d (0) {
    }
    ["d"]=>
    int(7)
    ["test"]=>
    string(6) ">seven"
  }
  [4]=>
  object(stdClass)#%d (3) {
    ["_id"]=>
    object(MongoDB\BSON\ObjectId)#%d (0) {
    }
    ["d"]=>
    int(8)
    ["test"]=>
    string(6) ">seven"
  }
  [5]=>
  object(stdClass)#%d (3) {
    ["_id"]=>
    object(MongoDB\BSON\ObjectId)#%d (0) {
    }
    ["d"]=>
    int(9)
    ["test"]=>
    string(6) ">seven"
  }
  [6]=>
  object(stdClass)#%d (3) {
    ["_id"]=>
    object(MongoDB\BSON\ObjectId)#%d (0) {
    }
    ["d"]=>
    int(10)
    ["test"]=>
    string(3) "ten"
  }
}
