--TEST--
MongoDB\BSON\Binary BSON encode
--FILE--
<?php
$tests = [
	[ 'field' => new \MongoDB\BSON\Decimal("1234.5678") ],
	[ 'field' => new \MongoDB\BSON\Decimal("1234e5") ],
	[ 'field' => new \MongoDB\BSON\Decimal("1234.56e-78") ],
	[ 'field' => new \MongoDB\BSON\Decimal("1234.5678") ],
	[ 'field' => new \MongoDB\BSON\Decimal(-234.567) ],
];


foreach ( $tests as $test )
{
	$bson = MongoDB\BSON\fromPHP( $test );
	echo MongoDB\BSON\toJSON( $bson ), "\n";
}
?>
--EXPECTF--
{ "field" : { "$numberDecimal" : "1234.5678" } }
{ "field" : { "$numberDecimal" : "1.234E+8" } }
{ "field" : { "$numberDecimal" : "1.23456E-75" } }
{ "field" : { "$numberDecimal" : "1234.5678" } }
{ "field" : { "$numberDecimal" : "-234.567" } }
