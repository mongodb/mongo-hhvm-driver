<?php
var_dump(extension_loaded("mongodb"));

$m = new MongoDB\Manager("mongodb://localhost:27017");

$testClass = new StdClass;
$testClass->true = "ja";
$testClass->false = "nee";

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
];

$r = $m->executeInsert( 'demo.test', $doc );
var_dump( $r );
?>
