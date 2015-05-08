<?php

function show_obj_properties($obj, array $keys)
{
	foreach ( $keys as $key )
	{
		$funcName = 'get' . $key . 'count';
		echo $key, ': ', $obj->$funcName(), '; ';
	}
	echo "\n";
}

function cleanup( MongoDB\Driver\Manager $m, $dbname = 'demo', $colname = 'test' )
{
	$c = new MongoDB\Driver\Command( [ 'drop' => $colname ] );
	try
	{
		$m->executeCommand( $dbname, $c );
	}
	catch ( MongoDB\Driver\Exception\RuntimeException $e )
	{
		/* do nothing */
	}
}
