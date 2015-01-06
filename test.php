<?php
var_dump(extension_loaded("mongodb"));

$m = new MongoDB\Manager("mongodb://localhost:27017");

$doc = [
	'name' => 'Derick',
	'twitter' => 'derickr',
	'site' => 'http://derickretans.nl',
];

$r = $m->executeInsert( 'demo.test', $doc );
var_dump( $r );
?>
