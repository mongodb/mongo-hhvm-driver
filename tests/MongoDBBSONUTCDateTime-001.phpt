--TEST--
MongoDB\BSON\UTCDateTime::__construct()
--FILE--
<?php
$items = [
	NULL,
	1464788392987,
	1464788392000,
	new DateTime("2016-06-01 14:40:23.123456 UTC"),
	new DateTimeImmutable("2016-06-01 14:40:23.123456 UTC"),
];

foreach ( $items as $value )
{
	if ( $value !== NULL )
	{
		$u = new \MongoDB\BSON\UTCDateTime( $value );
	}
	else
	{
		$u = new \MongoDB\BSON\UTCDateTime();
	}

	echo $u, ": ", $u->toDateTime()->format( "Y-m-d\TH:i:s.u O" ), "\n";
}
?>
--EXPECTF--
%d: %d-%d-%dT%d:%d:%d.%d +0000
1464788392987: 2016-06-01T13:39:52.987000 +0000
1464788392000: 2016-06-01T13:39:52.000000 +0000
1464792023123: 2016-06-01T14:40:23.123000 +0000
1464792023123: 2016-06-01T14:40:23.123000 +0000
