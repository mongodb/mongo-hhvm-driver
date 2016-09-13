--TEST--
BSON Object Serialization
--FILE--
<?php
$tests = [
	"Binary" => new \MongoDB\BSON\Binary( "foobar", MongoDB\BSON\Binary::TYPE_GENERIC ),
	"Binary with empty name" => new \MongoDB\BSON\Binary( '', MongoDB\BSON\Binary::TYPE_GENERIC ),
	"Binary with '\\0' in name" => new \MongoDB\BSON\Binary( "\0foo", MongoDB\BSON\Binary::TYPE_GENERIC ),
	"Binary UUID" => new \MongoDB\BSON\Binary( hex2bin( '123e4567e89b12d3a456426655440000' ), MongoDB\BSON\Binary::TYPE_UUID ),
	"Binary MD5" => new \MongoDB\BSON\Binary( md5( 'foobar', true ), MongoDB\BSON\Binary::TYPE_MD5 ),

	"Decimal128 Positive" => new \MongoDB\BSON\Decimal128( "1234.5678" ),
	"Decimal128 Negative" => new \MongoDB\BSON\Decimal128( "-1234.5678" ),
	"Decimal128 Exponent (string)" => new \MongoDB\BSON\Decimal128( "1234.56e-78" ),
	"Decimal128 Exponent (float)" => new \MongoDB\BSON\Decimal128( 1234.56e-78 ),
	"Decimal128 Infinity" => new \MongoDB\BSON\Decimal128( INF ),
	"Decimal128 NAN" => new \MongoDB\BSON\Decimal128( "NaN" ),

	"Javascript No Scope" => new \MongoDB\BSON\Javascript( 'function foo(bar) { return bar; }', null ),
	"Javascript Empty Scope" => new \MongoDB\BSON\Javascript( 'function foo(bar) { return bar; }', [] ),
	"Javascript With Scope" => new \MongoDB\BSON\Javascript( 'function foo() { return foo; }', ['foo' => 42] ),
	"Javascript With Scope with BSON Type" => new \MongoDB\BSON\Javascript( 'function foo() { return id; }', ['id' => new MongoDB\BSON\ObjectId('53e2a1c40640fd72175d4603') ] ),

	"MaxKey" => new \MongoDB\BSON\MaxKey(),

	"MinKey" => new \MongoDB\BSON\MinKey(),

	"ObjectID" => new \MongoDB\BSON\ObjectID( '576c25db6118fd406e6e6471' ),

	"Regex" => new \MongoDB\BSON\Regex( 'regexp', 'i' ),

	"Timestamp" => new \MongoDB\BSON\Timestamp( 1234, 5678 ),
	"Timestamp (Max, Min)" => new \MongoDB\BSON\Timestamp( 2147483647, 0 ),
	"Timestamp (Min, Max)" => new \MongoDB\BSON\Timestamp( 0, 2147483647 ),
	"Timestamp (64-bit: Max, Min)" => new \MongoDB\BSON\Timestamp( 4294967295, 0 ),
	"Timestamp (64-bit: Min, Max)" => new \MongoDB\BSON\Timestamp( 0, 4294967295 ),

	"UTCDateTime Epoch" => new \MongoDB\BSON\UTCDateTime( '0' ),
	"UTCDateTime Small (String)" => new \MongoDB\BSON\UTCDateTime( '-1416445411987' ),
	"UTCDateTime Large (String)" => new \MongoDB\BSON\UTCDateTime( '1416445411987' ),
	"UTCDateTime Small (Float)" => new \MongoDB\BSON\UTCDateTime( -1416445411987 ),
	"UTCDateTime Large (Float)" => new \MongoDB\BSON\UTCDateTime( 1416445411987 ),
];

foreach ( $tests as $name => $test )
{
	echo $name, "\n";
	$doc = [
		'value' => $test
	];

	$s = serialize( $doc );
	$u = unserialize( $s );

	echo $s, ": ";
	echo ( var_export( $doc, true ) === var_export( $u, true ) ) ? "equal" : "not equal", "\n\n";
}
?>
--EXPECTF--
Binary
a:1:{s:5:"value";C:19:"MongoDB\BSON\Binary":45:{a:2:{s:4:"data";s:6:"foobar";s:4:"type";i:0;}}}: equal

Binary with empty name
a:1:{s:5:"value";C:19:"MongoDB\BSON\Binary":39:{a:2:{s:4:"data";s:0:"";s:4:"type";i:0;}}}: equal

Binary with '\0' in name
a:1:{s:5:"value";C:19:"MongoDB\BSON\Binary":43:{a:2:{s:4:"data";s:4:"%a";s:4:"type";i:0;}}}: equal

Binary UUID
a:1:{s:5:"value";C:19:"MongoDB\BSON\Binary":56:{a:2:{s:4:"data";s:16:"%s";s:4:"type";i:4;}}}: equal

Binary MD5
a:1:{s:5:"value";C:19:"MongoDB\BSON\Binary":56:{a:2:{s:4:"data";s:16:"%s";s:4:"type";i:5;}}}: equal

Decimal128 Positive
a:1:{s:5:"value";C:23:"MongoDB\BSON\Decimal128":32:{a:1:{s:3:"dec";s:9:"1234.5678";}}}: equal

Decimal128 Negative
a:1:{s:5:"value";C:23:"MongoDB\BSON\Decimal128":34:{a:1:{s:3:"dec";s:10:"-1234.5678";}}}: equal

Decimal128 Exponent (string)
a:1:{s:5:"value";C:23:"MongoDB\BSON\Decimal128":35:{a:1:{s:3:"dec";s:11:"1.23456E-75";}}}: equal

Decimal128 Exponent (float)
a:1:{s:5:"value";C:23:"MongoDB\BSON\Decimal128":35:{a:1:{s:3:"dec";s:11:"1.23456E-75";}}}: equal

Decimal128 Infinity
a:1:{s:5:"value";C:23:"MongoDB\BSON\Decimal128":31:{a:1:{s:3:"dec";s:8:"Infinity";}}}: equal

Decimal128 NAN
a:1:{s:5:"value";C:23:"MongoDB\BSON\Decimal128":26:{a:1:{s:3:"dec";s:3:"NaN";}}}: equal

Javascript No Scope
a:1:{s:5:"value";C:23:"MongoDB\BSON\Javascript":72:{a:2:{s:4:"code";s:33:"function foo(bar) { return bar; }";s:5:"scope";N;}}}: equal

Javascript Empty Scope
a:1:{s:5:"value";C:23:"MongoDB\BSON\Javascript":89:{a:2:{s:4:"code";s:33:"function foo(bar) { return bar; }";s:5:"scope";O:8:"stdClass":0:{}}}}: equal

Javascript With Scope
a:1:{s:5:"value";C:23:"MongoDB\BSON\Javascript":101:{a:2:{s:4:"code";s:30:"function foo() { return foo; }";s:5:"scope";O:8:"stdClass":1:{s:3:"foo";i:42;}}}}: equal

Javascript With Scope with BSON Type
a:1:{s:5:"value";C:23:"MongoDB\BSON\Javascript":176:{a:2:{s:4:"code";s:29:"function foo() { return id; }";s:5:"scope";O:8:"stdClass":1:{s:2:"id";C:21:"MongoDB\BSON\ObjectID":48:{a:1:{s:3:"oid";s:24:"53e2a1c40640fd72175d4603";}}}}}}: equal

MaxKey
a:1:{s:5:"value";C:19:"MongoDB\BSON\MaxKey":0:{}}: equal

MinKey
a:1:{s:5:"value";C:19:"MongoDB\BSON\MinKey":0:{}}: equal

ObjectID
a:1:{s:5:"value";C:21:"MongoDB\BSON\ObjectID":48:{a:1:{s:3:"oid";s:24:"576c25db6118fd406e6e6471";}}}: equal

Regex
a:1:{s:5:"value";C:18:"MongoDB\BSON\Regex":53:{a:2:{s:7:"pattern";s:6:"regexp";s:5:"flags";s:1:"i";}}}: equal

Timestamp
a:1:{s:5:"value";C:22:"MongoDB\BSON\Timestamp":60:{a:2:{s:9:"increment";s:4:"1234";s:9:"timestamp";s:4:"5678";}}}: equal

Timestamp (Max, Min)
a:1:{s:5:"value";C:22:"MongoDB\BSON\Timestamp":64:{a:2:{s:9:"increment";s:10:"2147483647";s:9:"timestamp";s:1:"0";}}}: equal

Timestamp (Min, Max)
a:1:{s:5:"value";C:22:"MongoDB\BSON\Timestamp":64:{a:2:{s:9:"increment";s:1:"0";s:9:"timestamp";s:10:"2147483647";}}}: equal

Timestamp (64-bit: Max, Min)
a:1:{s:5:"value";C:22:"MongoDB\BSON\Timestamp":64:{a:2:{s:9:"increment";s:10:"4294967295";s:9:"timestamp";s:1:"0";}}}: equal

Timestamp (64-bit: Min, Max)
a:1:{s:5:"value";C:22:"MongoDB\BSON\Timestamp":64:{a:2:{s:9:"increment";s:1:"0";s:9:"timestamp";s:10:"4294967295";}}}: equal

UTCDateTime Epoch
a:1:{s:5:"value";C:24:"MongoDB\BSON\UTCDateTime":34:{a:1:{s:12:"milliseconds";s:1:"0";}}}: equal

UTCDateTime Small (String)
a:1:{s:5:"value";C:24:"MongoDB\BSON\UTCDateTime":48:{a:1:{s:12:"milliseconds";s:14:"-1416445411987";}}}: equal

UTCDateTime Large (String)
a:1:{s:5:"value";C:24:"MongoDB\BSON\UTCDateTime":47:{a:1:{s:12:"milliseconds";s:13:"1416445411987";}}}: equal

UTCDateTime Small (Float)
a:1:{s:5:"value";C:24:"MongoDB\BSON\UTCDateTime":48:{a:1:{s:12:"milliseconds";s:14:"-1416445411987";}}}: equal

UTCDateTime Large (Float)
a:1:{s:5:"value";C:24:"MongoDB\BSON\UTCDateTime":47:{a:1:{s:12:"milliseconds";s:13:"1416445411987";}}}: equal
