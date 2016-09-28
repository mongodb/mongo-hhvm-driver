--TEST--
MongoDB\Driver\WriteResult::__debugInfo() with WriteConcernError as object
--SKIPIF--
skip Manual test, as HHVM tests only run on StandAlone, where 'w > 1' is not supported.
--FILE--
<?php
include dirname(__FILE__) . '/utils.inc';

$m = new MongoDB\Driver\Manager("mongodb://localhost:27017");
cleanup( $m );

$bw = new MongoDB\Driver\BulkWrite( [ 'ordered' => false ] );
$bw->insert( [ 'x' => 3 ] );

$w = new MongoDB\Driver\WriteConcern(30, 100);

try
{
	$result = $m->executeBulkWrite( 'demo.test', $bw, $w );
}
catch ( MongoDB\Driver\Exception\BulkWriteException $e )
{
	var_dump( $e->getWriteResult() );
}
?>
--EXPECTF--
object(MongoDB\Driver\WriteResult)#%d (%d) {
  %a
  ["writeConcernError"]=>
  object(MongoDB\Driver\WriteConcernError)#%d (%d) {
    ["message"]=>
    string(29) "Not enough data-bearing nodes"
    ["code"]=>
    int(100)
    ["info"]=>
    NULL
  }
  %a
}
