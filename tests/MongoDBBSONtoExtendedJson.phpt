--TEST--
MongoDB\BSON\toExtendedJSON
--FILE--
<?php
$tests = [
	[ 'bool' => true, 'null' => null ],
	[ 'int' => 7, 'int64' => 5 * 1024 * 1048576, 'float' => M_PI ],
	[ 'binary' => new MongoDB\BSON\Binary( "foo", 2 ) ],
	[ 'dec128' => new MongoDB\BSON\Decimal128( "128" ) ],
	
	[
		'jscript' => new MongoDB\BSON\JavaScript( "function foo()" ),
		'jsscope' => new MongoDB\BSON\JavaScript( "function foo()", [ 'var1' => 42 ] ),
	],

	[ 'oid' => new MongoDB\BSON\ObjectID() ],
	[ 'regex' => new MongoDB\BSON\Regex( "^water", "mi" ) ],
	[ 'timestamp' => new MongoDB\BSON\Timestamp( 0x1234, 0x5678 ) ],
	[ 'utcdt' => new MongoDB\BSON\UTCDateTime( strtotime( "2017-04-06 14:41:14 UTC" ) * 1000 ) ],

];

foreach ( $tests as $test )
{
	$bson = MongoDB\BSON\fromPHP( $test );
	echo MongoDB\BSON\toExtendedJSON( $bson ), "\n";
}
?>
--EXPECTF--
{ "bool" : true, "null" : null }
{ "int" : { "$numberInt" : "7" }, "int64" : { "$numberLong" : "5368709120"}, "float" : { "$numberDouble" : "3.1415926535898002214" } }
{ "binary" : { "$binary" : "Zm9v", "$type" : "02" } }
{ "dec128" : { "$numberDecimal" : "128" } }
{ "jscript" : { "$code" : "function foo()" }, "jsscope" : { "$code" : "function foo()", "$scope" : { "var1" : { "$numberInt" : "42" } } } }
{ "oid" : { "$oid" : "%s" } }
{ "regex" : { "$regex" : "^water", "$options" : "im" } }
{ "timestamp" : { "$timestamp" : "95073396068916" } }
{ "utcdt" : { "$date" : { "$numberLong" : "1491489674000" } } }
