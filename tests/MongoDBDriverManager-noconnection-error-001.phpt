--TEST--
MongoDB\Driver\Manager - no connection
--FILE--
<?php
$m = new MongoDB\Driver\Manager("mongodb://localhost:44444");

$c = new MongoDB\Driver\Command( [ 'drop' => 'test'] );

try {
	$m->executeCommand( 'demo', $c );
	echo "Expected exception not thrown\n";
}
catch ( MongoDB\Driver\ConnectionException $e )
{
	echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Failed to connect to target host: localhost:44444
