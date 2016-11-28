--TEST--
MongoDB\Driver\Manager: maxStalenessSeconds
--FILE--
<?php
/* Fails because the URL doesn't include a non-primary RP */
try {
	$manager = new MongoDB\Driver\Manager("mongodb://localhost/?maxstalenessseconds=1231");
} catch ( MongoDB\Driver\Exception\InvalidArgumentException $e ) {
	echo $e->getMessage(), "\n";
}
try {
	$manager = new MongoDB\Driver\Manager("mongodb://localhost/?maxStalenessSeconds=1231");
} catch ( MongoDB\Driver\Exception\InvalidArgumentException $e ) {
	echo $e->getMessage(), "\n";
}

/* Fails because the URL doesn't include a non-primary RP, nor is a non-primary RP option set */
try {
	$manager = new MongoDB\Driver\Manager("mongodb://localhost/", [ 'maxstalenessseconds' => 1231 ] );
} catch ( MongoDB\Driver\Exception\InvalidArgumentException $e ) {
	echo $e->getMessage(), "\n";
}
try {
	$manager = new MongoDB\Driver\Manager("mongodb://localhost/", [ 'maxStalenessSeconds' => 1231 ] );
} catch ( MongoDB\Driver\Exception\InvalidArgumentException $e ) {
	echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Failed to parse MongoDB URI: 'mongodb://localhost/?maxstalenessseconds=1231'
Failed to parse MongoDB URI: 'mongodb://localhost/?maxStalenessSeconds=1231'
Primary read preference mode conflicts with maxStalenessSeconds
Primary read preference mode conflicts with maxStalenessSeconds
