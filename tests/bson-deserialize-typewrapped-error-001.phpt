--TEST--
BSON serialization: wrong data types for type mappings
--FILE--
<?php
$tests = [
	[ 'types' => 'string' ],
	[ 'types' => [ 'Does not exist' => 'Foo' ] ],
	[ 'types' => [ 'Binary' => 43 ] ],
	[ 'types' => [ 'Decimal128' => 'EmptyDecimalWrapper' ] ],
];

class EmptyDecimalWrapper {}

foreach ( $tests as $test )
{
	try
	{
		$bson = MongoDB\BSON\fromPHP( [ 'random' => 'stuff' ] );
		$test = MongoDB\BSON\toPHP( $bson, $test );
	}
	catch ( MongoDB\Driver\Exception\InvalidArgumentException $e )
	{
		echo $e->getMessage(), "\n";
	}
}
?>
--EXPECT--
The 'types' key in the type map should be an array
The type 'Does not exist' is not supported in the type map
The typemap for type 'Binary' should be a string
Class EmptyDecimalWrapper does not implement MongoDB\BSON\TypeWrapper
