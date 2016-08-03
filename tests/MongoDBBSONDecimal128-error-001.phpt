--TEST--
MongoDB\BSON\Decimal128 value object: wrong format
--FILE--
<?php
$tests = [
	".",
];

foreach ( $tests as $test )
{
	try {
		echo "Testing '$test'\n";

		$d = new \MongoDB\BSON\Decimal128( $test );

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
Testing '.'
Error parsing Decimal128 string: .
