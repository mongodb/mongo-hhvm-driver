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
