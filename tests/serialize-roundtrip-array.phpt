--TEST--
Serialization: empty array and empty class
--FILE--
<?php
include 'tests/utils.inc';

$m = new MongoDB\Driver\Manager("mongodb://localhost:27017");
cleanup( $m );

$m->executeInsert( 'demo.test', [ 'd' => 1, 'empty_array' => [], 'empty_class' => new stdclass ] );

var_dump( $m->executeQuery( 'demo.test', new MongoDB\Driver\Query( [] ) )->toArray() );
?>
--EXPECTF--
array(1) {
  [0]=>
  object(stdClass)#%d (4) {
    ["_id"]=>
    object(MongoDB\BSON\ObjectID)#%d (1) {
      ["oid"]=>
      string(24) "%s"
    }
    ["d"]=>
    int(1)
    ["empty_array"]=>
    array(0) {
    }
    ["empty_class"]=>
    object(stdClass)#%d (0) {
    }
  }
}
