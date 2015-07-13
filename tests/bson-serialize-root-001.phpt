--TEST--
BSON serialization: root document
--FILE--
<?php

$obj1 = new stdClass;
$obj1->{"0"} = 'foo';

$obj2 = new stdClass;
$obj2->cheese = 'awesome';

$tests = [
	[ ],
	[ 'a' => [ 8, 5, 2, 3 ] ],
	$obj1,
	$obj2, 
];

foreach ( $tests as $test )
{
	$bson = MongoDB\BSON\fromPHP( $test );
	echo MongoDB\BSON\toJSON( $bson ), "\n";
}
?>
--EXPECT--
{ }
{ "a" : [ 8, 5, 2, 3 ] }
{ "0" : "foo" }
{ "cheese" : "awesome" }
