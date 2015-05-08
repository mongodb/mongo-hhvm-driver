--TEST--
MongoDB\Driver\Manager::executeInsert
--FILE--
<?php
include 'utils.inc';

$m = new MongoDB\Driver\Manager("mongodb://localhost:27017");
cleanup( $m );

/* Inserting some data */
show_obj_properties( $m->executeInsert( 'demo.test', [ 'd' => 2 ] ), [ 'inserted' ] );

var_dump( $m->executeQuery( 'demo.test', new MongoDB\Driver\Query( [] ) )->toArray() );
?>
--EXPECTF--
inserted: 1; 
array(1) {
  [0]=>
  object(stdClass)#%d (2) {
    ["_id"]=>
    object(MongoDB\BSON\ObjectId)#%d (0) {
    }
    ["d"]=>
    int(2)
  }
}
