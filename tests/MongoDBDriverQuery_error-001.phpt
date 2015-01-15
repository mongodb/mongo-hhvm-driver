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
Expected filter to be array or object, integer given
Expected filter to be array or object, double given
Expected filter to be array or object, boolean given
Expected filter to be array or object, NULL given
