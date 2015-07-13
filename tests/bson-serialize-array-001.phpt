--TEST--
BSON serialization: PHP array to BSON array
--FILE--
<?php

$tests = [
	[ 'a' => [ 8, 5, 2, 3 ] ],
	[ 'a' => [ 0 => 4, 1 => 9 ] ],
];

foreach ( $tests as $test )
{
	$bson = MongoDB\BSON\fromPHP( $test );
	echo MongoDB\BSON\toJSON( $bson ), "\n";
}
?>
--EXPECT--
{ "a" : [ 8, 5, 2, 3 ] }
{ "a" : [ 4, 9 ] }
