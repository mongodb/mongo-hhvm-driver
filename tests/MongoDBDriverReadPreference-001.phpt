--TEST--
MongoDB\Driver\ReadPreference::__construct
--FILE--
<?php
$data = [
	[ MongoDB\Driver\ReadPreference::RP_PRIMARY ],
	[ MongoDB\Driver\ReadPreference::RP_PRIMARY_PREFERRED ],
	[ MongoDB\Driver\ReadPreference::RP_PRIMARY_PREFERRED, [ [ 'dcs' => 'east' ] ] ],
	[ MongoDB\Driver\ReadPreference::RP_SECONDARY ],
	[ MongoDB\Driver\ReadPreference::RP_SECONDARY, [ [ 'dcs' => 'east' ] ] ],
	[ MongoDB\Driver\ReadPreference::RP_SECONDARY_PREFERRED ],
	[ MongoDB\Driver\ReadPreference::RP_SECONDARY_PREFERRED, [ [ 'dcs' => 'east' ] ] ],
	[ MongoDB\Driver\ReadPreference::RP_NEAREST ],
	[ MongoDB\Driver\ReadPreference::RP_NEAREST, [ [ 'dcs' => 'east' ] ] ],
];

foreach ($data as $item) {
	$tags = NULL;
	@list($rp, $tags) = $item;
	echo str_replace("\n", "", var_export( $item, true ) ) . ":\n";
	try {
		if ($tags) {
			$w = new MongoDB\Driver\ReadPreference($rp, $tags);
		} else {
			$w = new MongoDB\Driver\ReadPreference($rp);
		}
	} catch (\InvalidArgumentException $e) {
		echo $e->getMessage(), "\n";
	}
}
?>
--EXPECT--
array (  0 => 1,):
array (  0 => 5,):
array (  0 => 5,  1 =>   array (    0 =>     array (      'dcs' => 'east',    ),  ),):
array (  0 => 2,):
array (  0 => 2,  1 =>   array (    0 =>     array (      'dcs' => 'east',    ),  ),):
array (  0 => 6,):
array (  0 => 6,  1 =>   array (    0 =>     array (      'dcs' => 'east',    ),  ),):
array (  0 => 10,):
array (  0 => 10,  1 =>   array (    0 =>     array (      'dcs' => 'east',    ),  ),):
