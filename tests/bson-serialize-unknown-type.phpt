--TEST--
BSON serialization: Unknown BSON Type class
--FILE--
<?php
class UnknownType implements MongoDB\BSON\Type {}

$tests = [
	[ 'x' => new UnknownType ],
];

foreach ( $tests as $test )
{
	try
	{
		$bson = MongoDB\BSON\fromPHP( $test );
	}
	catch ( MongoDB\Driver\Exception\UnexpectedValueException $e )
	{
		echo $e->getMessage(), "\n";
	}
}
?>
--EXPECT--
Unexpected MongoDB\BSON\Type instance: UnknownType
