--TEST--
MongoDB\Driver\Manager: maxStalenessSeconds (seconds too low)
--FILE--
<?php
/* Fails because the URL doesn't include a non-primary RP (is not validated by libmongoc) */
echo "URL has Secondary RP and seconds too low:\n";
try {
	$manager = new MongoDB\Driver\Manager("mongodb://localhost/?readpreference=SECONDARY&maxstalenessseconds=70");
	echo "not validated by libmongoc\n";
} catch ( MongoDB\Driver\Exception\InvalidArgumentException $e ) {
	echo $e->getMessage(), "\n";
}
try {
	$manager = new MongoDB\Driver\Manager("mongodb://localhost/?readPreference=SECONDARY&maxStalenessSeconds=70");
	echo "not validated by libmongoc\n";
} catch ( MongoDB\Driver\Exception\InvalidArgumentException $e ) {
	echo $e->getMessage(), "\n";
}

/* Fails because the URL doesn't include a non-primary RP, nor is a non-primary RP option set */
echo "URL has Secondary RP and options has seconds too low:\n";
try {
	$manager = new MongoDB\Driver\Manager("mongodb://localhost/?readpreference=SECONDARY", [ 'maxstalenessseconds' => 70 ] );
} catch ( MongoDB\Driver\Exception\InvalidArgumentException $e ) {
	echo $e->getMessage(), "\n";
}
try {
	$manager = new MongoDB\Driver\Manager("mongodb://localhost/?readPreference=SECONDARY", [ 'maxStalenessSeconds' => 70 ] );
} catch ( MongoDB\Driver\Exception\InvalidArgumentException $e ) {
	echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
URL has Secondary RP and seconds too low:
not validated by libmongoc
not validated by libmongoc
URL has Secondary RP and options has seconds too low:
Expected maxStalenessSeconds to be >= 90, 70 given
Expected maxStalenessSeconds to be >= 90, 70 given
