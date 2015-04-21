--TEST--
Serialization: nested objects
--FILE--
<?php
$m = new MongoDB\Driver\Manager("mongodb://localhost:27017");

$c = new MongoDB\Driver\Command( [ 'drop' => 'test'] );
$m->executeCommand( 'demo', $c );

$map1 = new stdClass;
$map1->foo = 42;

$map2 = new stdClass;
$map2->bar = $map1;

$m->executeInsert( 'demo.test', [ 'd' => 1, 'empty_class' => new stdclass ] );
$m->executeInsert( 'demo.test', [ 'd' => 2, 'empty_class' => $map1 ] );
$m->executeInsert( 'demo.test', [ 'd' => 3, 'empty_class' => $map2 ] );

var_dump($m->executeQuery( 'demo.test', new MongoDB\Driver\Query( [] ) )->toArray() );
?>
--EXPECT--
array(3) {
  [0]=>
  object(stdClass)#10 (3) {
    ["_id"]=>
    object(MongoDB\BSON\ObjectId)#8 (0) {
    }
    ["d"]=>
    int(1)
    ["empty_class"]=>
    object(stdClass)#9 (0) {
    }
  }
  [1]=>
  object(stdClass)#13 (3) {
    ["_id"]=>
    object(MongoDB\BSON\ObjectId)#11 (0) {
    }
    ["d"]=>
    int(2)
    ["empty_class"]=>
    object(stdClass)#12 (1) {
      ["foo"]=>
      int(42)
    }
  }
  [2]=>
  object(stdClass)#17 (3) {
    ["_id"]=>
    object(MongoDB\BSON\ObjectId)#14 (0) {
    }
    ["d"]=>
    int(3)
    ["empty_class"]=>
    object(stdClass)#16 (1) {
      ["bar"]=>
      object(stdClass)#15 (1) {
        ["foo"]=>
        int(42)
      }
    }
  }
}
