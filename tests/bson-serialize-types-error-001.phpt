--TEST--
BSON serialization: BSON types as root elements
--FILE--
<?php
$tests = [
	new MongoDB\BSON\ObjectID(),
	new MongoDB\BSON\MinKey(),
	new MongoDB\BSON\UTCDateTime("0"),
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
MongoDB\BSON\Type instance MongoDB\BSON\ObjectID cannot be serialized as a root element
MongoDB\BSON\Type instance MongoDB\BSON\MinKey cannot be serialized as a root element
MongoDB\BSON\Type instance MongoDB\BSON\UTCDateTime cannot be serialized as a root element
