--TEST--
MongoDB\Driver\Manager::executeBulkWrite should not execute more than once
--FILE--
<?php
include dirname(__FILE__) . '/utils.inc';

$m = new MongoDB\Driver\Manager("mongodb://localhost:27017");
cleanup( $m );

$bw = new MongoDB\Driver\BulkWrite( [ 'ordered' => false ] );
$_id = $bw->update( [ 'test' => 'foo', 'i' => 1 ], [ '$inc' => [ 'i' => 1 ] ] );

$result = $m->executeBulkWrite( 'demo.test', $bw );
try
{
	$result = $m->executeBulkWrite( 'demo.test', $bw );
}
catch ( MongoDB\Driver\Exception\Exception $e )
{
	echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
BulkWrite objects may only be executed once and this instance has already been executed
