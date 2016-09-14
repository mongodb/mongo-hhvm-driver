--TEST--
MongoDB\Driver\ReadPreference serialization
--FILE--
<?php
$tests = [
	new \MongoDB\Driver\ReadPreference( \MongoDB\Driver\ReadPreference::RP_PRIMARY ),
	new \MongoDB\Driver\ReadPreference( \MongoDB\Driver\ReadPreference::RP_SECONDARY, [ [ 'foo' => 'bar' ] ] ),
	new \MongoDB\Driver\ReadPreference( \MongoDB\Driver\ReadPreference::RP_SECONDARY_PREFERRED, [ [ 'foo' => 'bar' ], [] ] ),
];

foreach ( $tests as $test )
{
	var_dump( MongoDB\BSON\toJson( MongoDB\BSON\fromPHP( [ 'rp' => $test ] ) ) );
}
?>
--EXPECTF--
string(33) "{ "rp" : { "mode" : "primary" } }"
string(67) "{ "rp" : { "mode" : "secondary", "tags" : [ { "foo" : "bar" } ] } }"
string(82) "{ "rp" : { "mode" : "secondaryPreferred", "tags" : [ { "foo" : "bar" }, [  ] ] } }"
