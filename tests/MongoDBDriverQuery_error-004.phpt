--TEST--
MongoDB\Driver\Query::__construct: invalid arguments [4]
--FILE--
<?php
$data = [
	1, 3.14, true, null
];

foreach ($data as $item) {
	try {
		$w = new MongoDB\Driver\Query([], ['sort' => $item]);
	} catch (\InvalidArgumentException $e) {
		echo $e->getMessage(), "\n";
	}
}
?>
--EXPECT--
Expected sort to be array or object, integer given
Expected sort to be array or object, double given
Expected sort to be array or object, boolean given
Expected sort to be array or object, NULL given
