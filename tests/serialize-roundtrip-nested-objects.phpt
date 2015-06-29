--TEST--
Serialization: nested objects
--FILE--
<?php
include 'tests/utils.inc';

$m = new MongoDB\Driver\Manager("mongodb://localhost:27017");
cleanup( $m );

$map1 = new stdClass;
$map1->foo = 42;

$map2 = new stdClass;
$map2->bar = $map1;

$m->executeInsert( 'demo.test', [ 'd' => 1, 'empty_class' => new stdclass ] );
$m->executeInsert( 'demo.test', [ 'd' => 2, 'empty_class' => $map1 ] );
$m->executeInsert( 'demo.test', [ 'd' => 3, 'empty_class' => $map2 ] );

var_dump($m->executeQuery( 'demo.test', new MongoDB\Driver\Query( [] ) )->toArray() );
?>
--EXPECTF--
array(3) {
  [0]=>
  object(stdClass)#%d (3) {
    ["_id"]=>
    object(MongoDB\BSON\ObjectID)#%d (0) {
    }
    ["d"]=>
    int(1)
    ["empty_class"]=>
    object(stdClass)#%d (0) {
    }
  }
  [1]=>
  object(stdClass)#%d (3) {
    ["_id"]=>
    object(MongoDB\BSON\ObjectID)#%d (0) {
    }
    ["d"]=>
    int(2)
    ["empty_class"]=>
    object(stdClass)#%d (1) {
      ["foo"]=>
      int(42)
    }
  }
  [2]=>
  object(stdClass)#%d (3) {
    ["_id"]=>
    object(MongoDB\BSON\ObjectID)#%d (0) {
    }
    ["d"]=>
    int(3)
    ["empty_class"]=>
    object(stdClass)#%d (1) {
      ["bar"]=>
      object(stdClass)#%d (1) {
        ["foo"]=>
        int(42)
      }
    }
  }
}
