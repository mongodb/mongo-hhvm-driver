--TEST--
BSON serialization: PHP array to BSON document
--FILE--
<?php

$tests = [
	[ 'a' => [ 0 => 1, 2 => 8, 3 => 12 ] ],
	[ 'a' => [ "foo" => 42 ] ],
	[ 'a' => [ 1 => 9, 0 => 10 ] ],
];

foreach ( $tests as $test )
{
	$bson = MongoDB\BSON\fromPHP( $test );
	echo MongoDB\BSON\toJSON( $bson ), "\n";
}
?>
--EXPECT--
{ "a" : { "0" : 1, "2" : 8, "3" : 12 } }
{ "a" : { "foo" : 42 } }
{ "a" : { "1" : 9, "0" : 10 } }
