--TEST--
MongoDB\Driver\WriteResult::__debugInfo() with WriteError as object
--FILE--
<?php
require_once __DIR__ . "/utils.inc";

$manager = new MongoDB\Driver\Manager();
cleanup($manager);

$insertBulk = new MongoDB\Driver\BulkWrite;
$insertBulk->insert(['_id' => 1]);
$insertBulk->insert(['_id' => 1]);

try {
    $result = $manager->executeBulkWrite('demo.test', $insertBulk);
    printf("Inserted %d document(s)\n", $result->getInsertedCount());
} catch (MongoDB\Driver\Exception\BulkWriteException $e) {
    printf("Write error: %s\n", $e->getWriteResult()->getWriteErrors()[0]->getMessage());
	$wr = $e->getWriteResult();
	var_dump( $wr );
}
?>
--EXPECTF--
Write error: E11000 duplicate key error%s
object(MongoDB\Driver\WriteResult)#%d (%d) {
  %a
  ["writeErrors"]=>
  array(1) {
    [0]=>
    object(MongoDB\Driver\WriteError)#%d (%d) {
      ["message"]=>
      string(%d) "E11000 duplicate key error%s"
      ["code"]=>
      int(11000)
      ["index"]=>
      int(1)
      ["info"]=>
      NULL
    }
  }
  %a
}
