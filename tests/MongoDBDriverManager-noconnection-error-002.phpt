--TEST--
MongoDB\Driver\Manager - no connection (query)
--FILE--
<?php
$m = new MongoDB\Driver\Manager("mongodb://localhost:44444/?serverselectiontimeoutms=500");

$c = new MongoDB\Driver\Query( [] );

try {
	$m->executeQuery( 'demo.test', $c );
	echo "Expected exception not thrown\n";
}
catch ( MongoDB\Driver\Exception\ConnectionTimeoutException $e )
{
	echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Timed out trying to select a server
