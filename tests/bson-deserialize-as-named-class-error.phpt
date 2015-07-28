--TEST--
BSON deserialization: unserialize as named class (errors)
--FILE--
<?php
class NoSerialize
{
}

interface MyInterface extends MongoDB\BSON\Unserializable
{
}

$tests = [
	'{ "foo" : "yes" }',
];

$typemaps = [
	[ 'root' => 'NoSerialize' ],
	[ 'root' => 'DoesNotExist' ],
	[ 'root' => 'LiquidClass' ],
];

foreach ( $typemaps as $typemap )
{
	echo "root => ", $typemap['root'], "\n";

	foreach ( $tests as $test )
	{
		echo $test, "\n";
		$bson = MongoDB\BSON\fromJSON( $test );
		try
		{
			var_dump( MongoDB\BSON\toPHP( $bson, $typemap ) );
		}
		catch ( Exception $e )
		{
			echo $e->getMessage(), "\n";
		}
		echo "\n";
	}
}
?>
--EXPECT--
root => NoSerialize
{ "foo" : "yes" }
The typemap does not provide a class that implements MongoDB\BSON\Unserializable

root => DoesNotExist
{ "foo" : "yes" }
The typemap does not provide a class that implements MongoDB\BSON\Unserializable

root => LiquidClass
{ "foo" : "yes" }
The typemap does not provide a class that implements MongoDB\BSON\Unserializable
