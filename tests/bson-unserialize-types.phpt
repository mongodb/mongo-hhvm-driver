--TEST--
BSON Object Deserialization: Incomplete
--FILE--
<?php
$tests = [
	'Binary' => [
		'a:1:{s:5:"value";C:19:"MongoDB\BSON\Binary":31:{a:1:{s:4:"data";s:6:"foobar";}}}',
		'a:1:{s:5:"value";C:19:"MongoDB\BSON\Binary":22:{a:1:{s:4:"type";i:0;}}}',
		'a:1:{s:5:"value";C:19:"MongoDB\BSON\Binary":46:{a:2:{s:4:"data";i:0;s:4:"type";s:6:"foobar";}}}',
		'a:1:{s:5:"value";C:19:"MongoDB\BSON\Binary":47:{a:2:{s:4:"data";s:6:"foobar";s:4:"type";i:-1;}}}',
		'a:1:{s:5:"value";C:19:"MongoDB\BSON\Binary":48:{a:2:{s:4:"data";s:6:"foobar";s:4:"type";i:256;}}}',
	],

	'Decimal128' => [
		'a:1:{s:5:"value";C:23:"MongoDB\BSON\Decimal128":20:{a:1:{s:3:"dec";i:0;}}}',
		'a:1:{s:5:"value";C:23:"MongoDB\BSON\Decimal128":31:{a:1:{s:3:"dec";s:7:"INVALID";}}}',
	],

	'Javascript' => [
		'a:1:{s:5:"value";C:23:"MongoDB\BSON\Javascript":21:{a:1:{s:4:"code";i:0;}}}',
		'a:1:{s:5:"value";C:23:"MongoDB\BSON\Javascript":84:{a:2:{s:4:"code";s:33:"function foo(bar) { return bar; }";s:5:"scope";s:7:"INVALID";}}}',
	],

	'ObjectID' => [
		'a:1:{s:5:"value";C:21:"MongoDB\BSON\ObjectID":21:{a:1:{s:3:"oid";i:0;}}}',
		'a:1:{s:5:"value";C:21:"MongoDB\BSON\ObjectID":48:{a:1:{s:3:"oid";s:24:"0123456789abcdefghijklmn";}}}',
		'a:1:{s:5:"value";C:21:"MongoDB\BSON\ObjectID":31:{a:1:{s:3:"oid";s:7:"INVALID";}}}',
	],

	'Regex' => [
		'a:1:{s:5:"value";C:18:"MongoDB\BSON\Regex":34:{a:1:{s:7:"pattern";s:6:"regexp";}}}',
		'a:1:{s:5:"value";C:18:"MongoDB\BSON\Regex":27:{a:1:{s:5:"flags";s:1:"i";}}}',
		'a:1:{s:5:"value";C:18:"MongoDB\BSON\Regex":41:{a:2:{s:7:"pattern";i:0;s:5:"flags";i:0;}}}',
	],

	'Timestamp' => [
		'a:1:{s:5:"value";C:22:"MongoDB\BSON\Timestamp":30:{a:1:{s:9:"increment";i:1234;}}}',
		'a:1:{s:5:"value";C:22:"MongoDB\BSON\Timestamp":30:{a:1:{s:9:"timestamp";i:5678;}}}',
		'a:1:{s:5:"value";C:22:"MongoDB\BSON\Timestamp":47:{a:2:{s:9:"increment";N;s:9:"timestamp";i:5678;}}}',
		'a:1:{s:5:"value";C:22:"MongoDB\BSON\Timestamp":47:{a:2:{s:9:"increment";i:1234;s:9:"timestamp";N;}}}',
		'a:1:{s:5:"value";C:22:"MongoDB\BSON\Timestamp":50:{a:2:{s:9:"increment";i:-1;s:9:"timestamp";i:5678;}}}',
		'a:1:{s:5:"value";C:22:"MongoDB\BSON\Timestamp":60:{a:2:{s:9:"increment";i:-2147483647;s:9:"timestamp";i:5678;}}}',
		'a:1:{s:5:"value";C:22:"MongoDB\BSON\Timestamp":51:{a:2:{s:9:"increment";i:1234;s:9:"timestamp";i:-1;}}}',
		'a:1:{s:5:"value";C:22:"MongoDB\BSON\Timestamp":60:{a:2:{s:9:"increment";i:1234;s:9:"timestamp";i:-2147483647;}}}',
		'a:1:{s:5:"value";C:22:"MongoDB\BSON\Timestamp":59:{a:2:{s:9:"increment";i:4294967296;s:9:"timestamp";i:5678;}}}',
		'a:1:{s:5:"value";C:22:"MongoDB\BSON\Timestamp":59:{a:2:{s:9:"increment";i:1234;s:9:"timestamp";i:4294967296;}}}',
	],

	'UTCDateTime' => [
		'a:1:{s:5:"value";C:24:"MongoDB\BSON\UTCDateTime":30:{a:1:{s:12:"milliseconds";d:1;}}}',
		'a:1:{s:5:"value";C:24:"MongoDB\BSON\UTCDateTime":40:{a:1:{s:12:"milliseconds";s:7:"INVALID";}}}',
	],
];

foreach ( $tests as $name => $section )
{
	echo $name, ":\n";
	foreach ( $section as $test )
	{
		try
		{
			$u = unserialize( $test );
			echo "Expected exception not thrown\n";
		}
		catch ( Exception $e )
		{
			echo get_class( $e ), ': ', $e->getMessage(), "\n";
		}
	}
	echo "\n";
}
?>
--EXPECTF--
Binary:
MongoDB\Driver\Exception\InvalidArgumentException: MongoDB\BSON\Binary initialization requires "data" string and "type" integer fields
MongoDB\Driver\Exception\InvalidArgumentException: MongoDB\BSON\Binary initialization requires "data" string and "type" integer fields
MongoDB\Driver\Exception\InvalidArgumentException: MongoDB\BSON\Binary initialization requires "data" string and "type" integer fields
MongoDB\Driver\Exception\InvalidArgumentException: Expected type to be an unsigned 8-bit integer, -1 given
MongoDB\Driver\Exception\InvalidArgumentException: Expected type to be an unsigned 8-bit integer, 256 given

Decimal128:
MongoDB\Driver\Exception\InvalidArgumentException: MongoDB\BSON\Decimal128 initialization requires "dec" string field
MongoDB\Driver\Exception\InvalidArgumentException: Error parsing Decimal128 string: INVALID

Javascript:
MongoDB\Driver\Exception\InvalidArgumentException: MongoDB\BSON\Javascript initialization requires "code" string field
MongoDB\Driver\Exception\InvalidArgumentException: Expected scope to be array or object, string given

ObjectID:
MongoDB\Driver\Exception\InvalidArgumentException: MongoDB\BSON\ObjectID initialization requires "oid" string field
MongoDB\Driver\Exception\InvalidArgumentException: Error parsing ObjectID string: 0123456789abcdefghijklmn
MongoDB\Driver\Exception\InvalidArgumentException: Error parsing ObjectID string: INVALID

Regex:
MongoDB\Driver\Exception\InvalidArgumentException: MongoDB\BSON\Regex initialization requires "pattern" and "flags" string fields
MongoDB\Driver\Exception\InvalidArgumentException: MongoDB\BSON\Regex initialization requires "pattern" and "flags" string fields
MongoDB\Driver\Exception\InvalidArgumentException: MongoDB\BSON\Regex initialization requires "pattern" and "flags" string fields

Timestamp:
MongoDB\Driver\Exception\InvalidArgumentException: MongoDB\BSON\Timestamp initialization requires "increment" and "timestamp" integer or numeric string fields
MongoDB\Driver\Exception\InvalidArgumentException: MongoDB\BSON\Timestamp initialization requires "increment" and "timestamp" integer or numeric string fields
MongoDB\Driver\Exception\InvalidArgumentException: MongoDB\BSON\Timestamp initialization requires "increment" and "timestamp" integer or numeric string fields
MongoDB\Driver\Exception\InvalidArgumentException: MongoDB\BSON\Timestamp initialization requires "increment" and "timestamp" integer or numeric string fields
MongoDB\Driver\Exception\InvalidArgumentException: Expected increment to be an unsigned 32-bit integer, -1 given
MongoDB\Driver\Exception\InvalidArgumentException: Expected increment to be an unsigned 32-bit integer, -2147483647 given
MongoDB\Driver\Exception\InvalidArgumentException: Expected timestamp to be an unsigned 32-bit integer, -1 given
MongoDB\Driver\Exception\InvalidArgumentException: Expected timestamp to be an unsigned 32-bit integer, -2147483647 given
MongoDB\Driver\Exception\InvalidArgumentException: Expected increment to be an unsigned 32-bit integer, 4294967296 given
MongoDB\Driver\Exception\InvalidArgumentException: Expected timestamp to be an unsigned 32-bit integer, 4294967296 given

UTCDateTime:
MongoDB\Driver\Exception\InvalidArgumentException: MongoDB\BSON\UTCDateTime initialization requires "milliseconds" integer or numeric string field
MongoDB\Driver\Exception\InvalidArgumentException: Error parsing "INVALID" as 64-bit integer for MongoDB\BSON\UTCDateTime initialization
