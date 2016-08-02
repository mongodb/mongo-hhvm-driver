--TEST--
MongoDB\BSON\ObjectID value object: wrong format
--FILE--
<?php
$tests = [
	"28734",
];

foreach ( $tests as $test )
{
	try {
		echo "Testing '$test'\n";

		$d = new \MongoDB\BSON\ObjectID( $test );

		var_dump( $d );
		echo (string) $d, "\n";
	}
	catch ( \MongoDB\Driver\Exception\InvalidArgumentException $e )
	{
		echo $e->getMessage(), "\n";
	}
	echo "\n";
}
?>
--EXPECTF--
Testing '28734'
Error parsing ObjectID string: 28734
