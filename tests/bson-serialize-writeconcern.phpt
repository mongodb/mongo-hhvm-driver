--TEST--
MongoDB\Driver\WriteConcern serialization
--FILE--
<?php
$tests = [
	new \MongoDB\Driver\WriteConcern( -2 ), // Magic value which means "DEFAULT"
	new \MongoDB\Driver\WriteConcern( 0 ),
	new \MongoDB\Driver\WriteConcern( 2 ),
	new \MongoDB\Driver\WriteConcern( \MongoDB\Driver\WriteConcern::MAJORITY ),
	new \MongoDB\Driver\WriteConcern( 2, 10000 ),
	new \MongoDB\Driver\WriteConcern( 2, 10000, true ),
];

foreach ( $tests as $test )
{
	var_dump( MongoDB\BSON\toJson( MongoDB\BSON\fromPHP( [ 'wc' => $test ] ) ) );
}
?>
--EXPECTF--
string(15) "{ "wc" : {  } }"
string(22) "{ "wc" : { "w" : 0 } }"
string(22) "{ "wc" : { "w" : 2 } }"
string(31) "{ "wc" : { "w" : "majority" } }"
string(42) "{ "wc" : { "w" : 2, "wtimeout" : 10000 } }"
string(54) "{ "wc" : { "w" : 2, "j" : true, "wtimeout" : 10000 } }"

