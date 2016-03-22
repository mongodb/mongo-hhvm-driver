--TEST--
MongoDB\Driver\Manager::executeBulkWrite() update write error
--FILE--
<?php
include dirname(__FILE__) . '/utils.inc';

$m = new MongoDB\Driver\Manager("mongodb://localhost:27017");
cleanup( $m );

$bw = new MongoDB\Driver\BulkWrite();

try {
	$result = $m->executeBulkWrite( 'demo.test', $bw );
} catch ( Exception $e ) {
	echo get_class( $e ), "\n";
	echo $e->getMessage(), "\n";
}

?>
--EXPECT--
MongoDB\Driver\Exception\InvalidArgumentException
Cannot execute an empty BulkWrite
