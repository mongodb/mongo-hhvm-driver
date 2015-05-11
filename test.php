<?php
var_dump(extension_loaded("mongodb"));

try
{
	$falseOid = new BSON\ObjectId('xx1234567890y1234567890z');
}
catch ( InvalidArgumentException $e )
{
	echo $e->getMessage(), "\n";
}

$m = new MongoDB\Driver\Manager("mongodb://localhost:27017");

$c = new MongoDB\Driver\Command( [ 'drop' => 'test'] );
try {
	$cursor = $m->executeCommand( 'demo', $c );

	foreach( $cursor->toArray() as $result )
	{
		var_dump($result);
	}
}
catch ( InvalidArgumentException $e )
{
	echo $e->getMessage(), "\n";
}

echo "================\n";

$map = new StdClass;
$map->bar = 52;
$map->foo = true;

$doc = [
	'string' => 'bar',
	'number_i' => 55,
	'number_l' => 12345678901234567,
	'bool' => true,
	'null' => null,
	'float' => M_PI,
	'binary' => new BSON\Binary("random binary\0data", BSON\Binary::TYPE_MD5),
	'oid1' => new BSON\ObjectId(),
	'oid2' => new BSON\ObjectId('456712341111222222333333'),
	'maxkey' => new BSON\MaxKey(),
	'minkey' => new BSON\MinKey(),
	'utcdatetime' => new BSON\UtcDatetime(time() * 1000),
	'code' => new BSON\Javascript("function() { return x + 1; }"),
	'codeWithScope1' => new BSON\Javascript("function() { return x + 1; }", [ 'foo' => 42, 'bar' => M_PI ] ),
	'codeWithScope2' => new BSON\Javascript("function() { return x + 1; }", $map ),
	'codeWithScope3' => new BSON\Javascript("function() { return x + 1; }", 42 ),
	'timestamp' => new BSON\Timestamp(1234, 5678),
	'regex' => new BSON\Regex('derick', 'im'),
	'array' => [ 1, 5, 'foo', M_PI, true ],
	'document' => $map,
	'arraydocument' => [ 0, "foo" => 5, $map, $map ],
];

$m->executeInsert( 'demo.test', [ 'empty_array' => [], 'empty_class' => new stdclass ] );

for ( $i = 0; $i < 2; $i++ )
{
	$r = $m->executeInsert( 'demo.test', $doc );
	$r = $m->executeInsert( 'demo.test', $doc );
	$r = $m->executeInsert( 'demo.test', $doc );
	$r = $m->executeInsert( 'demo.test', $doc );
	$r = $m->executeInsert( 'demo.test', $doc );
}
$m->executeInsert( 'demo.test', [ 'without_id' => true ] );
$m->executeInsert( 'demo.test', [ '_id' => new Bson\ObjectId(), 'with_id' => true ] );

$q = new MongoDB\Driver\Query( [] );

$cursor = $m->executeQuery( 'demo.test', $q );

echo "Starting iteration\n";

foreach ( $cursor->toArray() as $key => $result )
{
	echo $result['_id'], ":\n";
	echo ' - ', $result['oid1'], "\n";
	echo ' - ', $result['oid2'], "\n";
	var_dump($key, $result);
}
/*
echo $cursorId, "\n";
$cursorId = new MongoDB\Driver\CursorID("345345");
echo $cursorId, "\n";
*/
/*
$r = new MongoDB\Driver\ReadPreference(MongoDB\Driver\ReadPreference::RP_SECONDARY, [ [ 'cs' => 'east' ] ] );


class myClass {
	public $publiek = 1;
	protected $beveiligd = 2;
	private $privaat = 3;
}

$testClass = new StdClass;
$testClass->true = "ja";
$testClass->false = "nee";

$re = new BSON\Regex("^name=", "");

$doc = [
	'name' => 'Derick',
	'twitter' => 'derickr',
	'age' => 36.1,
	'length' => 185,
	'does_opensource' => true,
	'null_value' => null,
	'sites' => [ 'http://derickretans.nl', 'http://xdebug.org' ],
	'sites_err' => [ 0 => 'http://derickretans.nl', 2 => 'http://xdebug.org' ],
	'phones' => [ 'work' => '02077771111', 'mobile' => '075551111' ],
	'boolean' => $testClass,
	'ppp' => new MyClass,
	're' => $re,
];

$r = $m->executeInsert( 'demo.test', new MyClass );
$r = $m->executeInsert( 'demo.test', $doc );

$w = new MongoDB\Driver\Query(
	['twitter' => 'derickr'],
	[
		'projection' => ['phones' => 1, 'ppp' => 1 ],
		'sort' => [ 'length' => 1 ]
	]
);

echo MongoDB\Driver\Query::FLAG_NONE, "\n";
*/
?>
