--TEST--
MongoDB\Driver\WriteResult::__debugInfo() with UpsertedIDs as object
--FILE--
<?php
include dirname(__FILE__) . '/utils.inc';

$m = new MongoDB\Driver\Manager("mongodb://localhost:27017");
cleanup( $m );

$bw = new MongoDB\Driver\BulkWrite( [ 'ordered' => false ] );
$bw->update( [ 'i' => 8 ], [ '$set' => [ 'new' => true ] ], [ 'upsert' => true ] );
$bw->update( [ 'i' => 9 ], [ '$set' => [ 'new' => true ] ], [ 'upsert' => true ] );

$result = $m->executeBulkWrite( 'demo.test', $bw );
var_dump( $result );
?>
--EXPECTF--
object(MongoDB\Driver\WriteResult)#%d (%d) {
  %a
  ["upsertedIds"]=>
  array(2) {
    [0]=>
    array(2) {
      ["index"]=>
      int(0)
      ["_id"]=>
      object(MongoDB\BSON\ObjectID)#%d (%d) {
        ["oid"]=>
        string(24) "%s"
      }
    }
    [1]=>
    array(2) {
      ["index"]=>
      int(1)
      ["_id"]=>
      object(MongoDB\BSON\ObjectID)#%d (%d) {
        ["oid"]=>
        string(24) "%s"
      }
    }
  }
  %a
}
