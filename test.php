<?php
var_dump(extension_loaded("mongodb"));

$m = new MongoDB\Manager("mongodb://localhost:27017");

$doc = [
	'name' => 'Derick',
	'twitter' => 'derickr',
	'age' => 36.1,
	'length' => 185,
	'does_opensource' => true,
	'null_value' => null,
	'sites' => [ 'http://derickretans.nl', 'http://xdebug.org' ],
];

$r = $m->executeInsert( 'demo.test', $doc );
var_dump( $r );
?>
