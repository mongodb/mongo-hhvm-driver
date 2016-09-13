--TEST--
MongoDB\Driver\WriteConcern serialization
--FILE--
<?php
$tests = [
	new \MongoDB\Driver\WriteConcern( \MongoDB\Driver\WriteConcern::MAJORITY ),
	new \MongoDB\Driver\WriteConcern( 1, NULL, true ),
	new \MongoDB\Driver\WriteConcern( 0, 80000, false ),
	new \MongoDB\Driver\WriteConcern( 2, 80000 ),
	new \MongoDB\Driver\WriteConcern( 2, 0, false ),
];

foreach ( $tests as $test )
{
	var_dump( MongoDB\BSON\toJson( MongoDB\BSON\fromPHP( [ 'wc' => $test ] ) ) );
}
?>
--EXPECTF--
string(31) "{ "wc" : { "w" : "majority" } }"
string(34) "{ "wc" : { "w" : 1, "j" : true } }"
string(55) "{ "wc" : { "w" : 0, "j" : false, "wtimeout" : 80000 } }"
string(42) "{ "wc" : { "w" : 2, "wtimeout" : 80000 } }"
string(35) "{ "wc" : { "w" : 2, "j" : false } }"
