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

/**
 * Prints a traditional hex dump of byte values and printable characters.
 *
 * @see http://stackoverflow.com/a/4225813/162228
 * @param string $data   Binary data
 * @param integer $width Bytes displayed per line
 */
function hex_dump($data, $width = 16)
{
	static $pad = '.'; // Placeholder for non-printable characters
	static $from = '';
	static $to = '';

	if ($from === '') {
		for ($i = 0; $i <= 0xFF; $i++) {
			$from .= chr($i);
			$to .= ($i >= 0x20 && $i <= 0x7E) ? chr($i) : $pad;
		}
	}

	$hex = str_split(bin2hex($data), $width * 2);
	$chars = str_split(strtr($data, $from, $to), $width);

	$offset = 0;
	$length = $width * 3;

	foreach ($hex as $i => $line) {
		printf("%6X : %-{$length}s [%s]\n", $offset, implode(' ', str_split($line, 2)), $chars[$i]);
		$offset += $width;
	}
}
?>
