--TEST--
MongoDB\Driver\ReadPreference::__construct: invalid arguments [1]
--FILE--
<?php
function test_error_handler($type, $message)
{
	echo "Catchable fatal error: ", $message, "\n";
}

set_error_handler('test_error_handler');

$data = [
	[ MongoDB\Driver\ReadPreference::RP_PRIMARY, 42 ],
	[ MongoDB\Driver\ReadPreference::RP_PRIMARY, [ 42 ] ],
	[ MongoDB\Driver\ReadPreference::RP_PRIMARY, [ 'dcs' => 'east' ] ],
	[ MongoDB\Driver\ReadPreference::RP_PRIMARY, [ [ 'dcs' => 'east' ] ] ],
	[ MongoDB\Driver\ReadPreference::RP_PRIMARY, [ 'invalid' ] ],
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
Catchable fatal error: Argument 2 passed to MongoDB\Driver\ReadPreference::__construct() must be an instance of array, int given
array (  0 => 1,  1 =>   array (    0 => 42,  ),):
tagSets may not be used with primary mode
array (  0 => 1,  1 =>   array (    'dcs' => 'east',  ),):
tagSets may not be used with primary mode
array (  0 => 1,  1 =>   array (    0 =>     array (      'dcs' => 'east',    ),  ),):
tagSets may not be used with primary mode
array (  0 => 1,  1 =>   array (    0 => 'invalid',  ),):
tagSets may not be used with primary mode
array (  0 => 99,  1 => 42,):
Catchable fatal error: Argument 2 passed to MongoDB\Driver\ReadPreference::__construct() must be an instance of array, int given
array (  0 => 99,  1 =>   array (    0 => 42,  ),):
Invalid mode: 99
array (  0 => 99,  1 =>   array (    'dcs' => 'east',  ),):
Invalid mode: 99
array (  0 => 99,  1 =>   array (    0 =>     array (      'dcs' => 'east',    ),  ),):
Invalid mode: 99
