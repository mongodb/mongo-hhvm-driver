--TEST--
MongoDB\Driver\Query::__construct: invalid arguments [2]
--FILE--
<?php
$data = [
	1, 3.14, true, null
];

foreach ($data as $item) {
	try {
		$w = new MongoDB\Driver\Query([], ['modifiers' => $item]);
	} catch (\InvalidArgumentException $e) {
		echo $e->getMessage(), "\n";
	}
}
?>
--EXPECTF--
Expected modifiers to be array or object, integer given
Expected modifiers to be array or object, double given
Expected modifiers to be array or object, boolean given
Expected modifiers to be array or object, %r(null|NULL)%r given
