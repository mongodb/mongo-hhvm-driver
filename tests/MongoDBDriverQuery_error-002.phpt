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
Expected "modifiers" option to be array, integer given
Expected "modifiers" option to be array, double given
Expected "modifiers" option to be array, boolean given
Expected "modifiers" option to be array, %r(null|NULL)%r given
