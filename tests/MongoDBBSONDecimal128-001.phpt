--TEST--
MongoDB\BSON\Decimal128 value object
--FILE--
<?php
$tests = [
	"1234.5678",
	"1234e5",
	"1234.56e-78",
	"-1234.5678",
	-234.567,
	acos(8),
	1.7976931348623158e+308+1.7976931348623158e+308,
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
Testing '1234.5678'
object(MongoDB\BSON\Decimal128)#%d (1) {
  ["decimal"]=>
  string(9) "1234.5678"
}
1234.5678

Testing '1234e5'
object(MongoDB\BSON\Decimal128)#%d (1) {
  ["decimal"]=>
  string(8) "1.234E+8"
}
1.234E+8

Testing '1234.56e-78'
object(MongoDB\BSON\Decimal128)#%d (1) {
  ["decimal"]=>
  string(11) "1.23456E-75"
}
1.23456E-75

Testing '-1234.5678'
object(MongoDB\BSON\Decimal128)#%d (1) {
  ["decimal"]=>
  string(10) "-1234.5678"
}
-1234.5678

Testing '-234.567'
object(MongoDB\BSON\Decimal128)#%d (1) {
  ["decimal"]=>
  string(8) "-234.567"
}
-234.567

Testing 'NAN'
Error parsing Decimal128 string: NAN

Testing 'INF'
object(MongoDB\BSON\Decimal128)#%d (1) {
  ["decimal"]=>
  string(3) "Inf"
}
Inf
