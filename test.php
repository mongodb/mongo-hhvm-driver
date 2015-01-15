<?php
var_dump(extension_loaded("mongodb"));

$m = new MongoDB\Manager("mongodb://localhost:27017");

class myClass {
	public $publiek = 1;
	protected $beveiligd = 2;
	private $privaat = 3;
}

$testClass = new StdClass;
$testClass->true = "ja";
$testClass->false = "nee";

$re = new MongoDB\BSON\Regex("^name=", "");

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

var_dump( $r );
?>
