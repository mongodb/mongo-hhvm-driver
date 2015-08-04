--TEST--
MongoDB\Driver\Query::__construct: invalid arguments [1]
--FILE--
<?php
$data = [
	1, 3.14, true, null
];

foreach ($data as $item) {
	try {
		$w = new MongoDB\Driver\Query($item);
	} catch (\InvalidArgumentException $e) {
		echo $e->getMessage(), "\n";
	}
}
?>
--EXPECT--
MongoDB\Driver\Query::__construct() expects filter to be array or object, integer given
MongoDB\Driver\Query::__construct() expects filter to be array or object, double given
MongoDB\Driver\Query::__construct() expects filter to be array or object, boolean given
MongoDB\Driver\Query::__construct() expects filter to be array or object, NULL given
