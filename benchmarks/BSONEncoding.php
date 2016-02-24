<?php
namespace MongoDB\Benchmark;

abstract class BSONEncoding extends Base
{
	function loadFile( $filename )
	{
		$data = null;

		foreach ( file( $filename ) as $line )
		{
			$data = json_decode( $line );
		}

		return $data;
	}
}
?>
