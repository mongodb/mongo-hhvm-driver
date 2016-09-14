--TEST--
MongoDB\Driver\ReadConcern serialization
--FILE--
<?php
$tests = [
	new \MongoDB\Driver\ReadConcern(),
	new \MongoDB\Driver\ReadConcern( \MongoDB\Driver\ReadConcern::LOCAL ),
	new \MongoDB\Driver\ReadConcern( \MongoDB\Driver\ReadConcern::MAJORITY ),
	new \MongoDB\Driver\ReadConcern( \MongoDB\Driver\ReadConcern::LINEARIZABLE ),
];

foreach ( $tests as $test )
{
	var_dump( MongoDB\BSON\toJson( MongoDB\BSON\fromPHP( [ 'rc' => $test ] ) ) );
}
?>
--EXPECTF--
string(15) "{ "rc" : [  ] }"
string(32) "{ "rc" : { "level" : "local" } }"
string(35) "{ "rc" : { "level" : "majority" } }"
string(39) "{ "rc" : { "level" : "linearizable" } }"
