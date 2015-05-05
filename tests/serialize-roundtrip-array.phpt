--TEST--
Serialization: empty array and empty class
--FILE--
<?php
$m = new MongoDB\Driver\Manager("mongodb://localhost:27017");

$c = new MongoDB\Driver\Command( [ 'drop' => 'test'] );
$m->executeCommand( 'demo', $c );

$m->executeInsert( 'demo.test', [ 'd' => 1, 'empty_array' => [], 'empty_class' => new stdclass ] );

var_dump( $m->executeQuery( 'demo.test', new MongoDB\Driver\Query( [] ) )->toArray() );
?>
--EXPECTF--
array(1) {
  [0]=>
  object(stdClass)#%d (4) {
    ["_id"]=>
    object(MongoDB\BSON\ObjectId)#%d (0) {
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
