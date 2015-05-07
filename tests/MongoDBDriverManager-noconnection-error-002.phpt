--TEST--
MongoDB\Driver\Manager - no connection (query)
--FILE--
<?php
$m = new MongoDB\Driver\Manager("mongodb://localhost:44444");

$c = new MongoDB\Driver\Query( [] );

try {
	$m->executeQuery( 'demo.test', $c );
	echo "Expected exception not thrown\n";
}
catch ( MongoDB\Driver\Exception\ConnectionException $e )
{
	echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Failed to connect to target host: localhost:44444
