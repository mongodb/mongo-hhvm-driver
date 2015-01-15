--TEST--
MongoDB\Driver\Query::__construct: invalid arguments [3]
--FILE--
<?php
$data = [
	1, 3.14, true, null
];

foreach ($data as $item) {
	try {
		$w = new MongoDB\Driver\Query([], ['projection' => $item]);
	} catch (\InvalidArgumentException $e) {
		echo $e->getMessage(), "\n";
	}
}
?>
--EXPECT--
Expected projection to be array or object, integer given
Expected projection to be array or object, double given
Expected projection to be array or object, boolean given
Expected projection to be array or object, NULL given
