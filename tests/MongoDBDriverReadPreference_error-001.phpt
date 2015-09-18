--TEST--
MongoDB\Driver\ReadPreference::__construct: invalid arguments [1]
--FILE--
<?php
$data = [
	[ MongoDB\Driver\ReadPreference::RP_PRIMARY, 42 ],
	[ MongoDB\Driver\ReadPreference::RP_PRIMARY, [ 42 ] ],
	[ MongoDB\Driver\ReadPreference::RP_PRIMARY, [ 'dcs' => 'east' ] ],
	[ MongoDB\Driver\ReadPreference::RP_PRIMARY, [ [ 'dcs' => 'east' ] ] ],
	[ 99, 42 ],
	[ 99, [ 42 ] ],
	[ 99, [ 'dcs' => 'east' ] ],
	[ 99, [ [ 'dcs' => 'east' ] ] ],
];

foreach ($data as $item) {
	list($rp, $tags) = $item;
	echo str_replace("\n", "", var_export( $item, true ) ) . ":\n";
	try {
		$w = new MongoDB\Driver\ReadPreference($rp, $tags);
	} catch (\InvalidArgumentException $e) {
		echo $e->getMessage(), "\n";
	}
}
?>
--EXPECT--
array (  0 => 1,  1 => 42,):
Expected parameter 2 to be array or object, integer given
array (  0 => 1,  1 =>   array (    0 => 42,  ),):
Invalid tagSets
array (  0 => 1,  1 =>   array (    'dcs' => 'east',  ),):
Invalid tagSets
array (  0 => 1,  1 =>   array (    0 =>     array (      'dcs' => 'east',    ),  ),):
Invalid tagSets
array (  0 => 99,  1 => 42,):
Expected parameter 2 to be array or object, integer given
array (  0 => 99,  1 =>   array (    0 => 42,  ),):
Invalid mode: 99
array (  0 => 99,  1 =>   array (    'dcs' => 'east',  ),):
Invalid mode: 99
array (  0 => 99,  1 =>   array (    0 =>     array (      'dcs' => 'east',    ),  ),):
Invalid mode: 99
