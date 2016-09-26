--TEST--
Sort query option is always serialized as a BSON document
--FILE--
<?php
require 'utils.inc';

$manager = new MongoDB\Driver\Manager();
cleanup( $manager );

$bulkWrite = new MongoDB\Driver\BulkWrite;

for ($i = 0; $i < 5; $i++) {
    $bulkWrite->insert(array('_id' => $i, '0' => 4 - $i));
}

$writeResult = $manager->executeBulkWrite('demo.test', $bulkWrite);
printf("Inserted: %d\n", $writeResult->getInsertedCount());

$query = new MongoDB\Driver\Query(array(), array(
    'sort' => array('0' => 1),
));

var_dump($query);

$cursor = $manager->executeQuery('demo.test', $query);

/* Numeric keys of stdClass instances cannot be directly accessed, so ensure the
 * document is decoded as a PHP array.
 */
$cursor->setTypeMap(array('root' => 'array'));

foreach ($cursor as $document) {
    echo $document['0'] . "\n";
}

?>
--EXPECTF--
Inserted: 5
object(MongoDB\Driver\Query)#%d (%d) {
  ["filter"]=>
  object(stdClass)#%d (%d) {
  }
  ["options"]=>
  object(stdClass)#%d (%d) {
    ["sort"]=>
    object(stdClass)#%d (%d) {
      [0]=>
      int(1)
    }
  }
  ["readConcern"]=>
  NULL
}
0
1
2
3
4
