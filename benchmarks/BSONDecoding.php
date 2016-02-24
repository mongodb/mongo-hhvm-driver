<?php
namespace MongoDB\Benchmark;

abstract class BSONDecoding extends Base
{
	function loadFile( $filename )
	{
		$data = null;

		foreach ( file( $filename ) as $line )
		{
			$data = \MongoDB\BSON\fromJSON( $line );
		}

		return $data;
	}
}
?>
