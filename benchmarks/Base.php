<?php
namespace MongoDB\Benchmark;

class Base
{
	function calculatePercentiles( $times )
	{
		$percentiles = [ 10, 25, 50, 75, 90, 95, 98, 99 ];
		$percentileTimes = [];

		$items = count( $times );
		sort( $times );

		foreach ( $percentiles as $percentile )
		{
			$index = (int) ( $items * $percentile / 100 );
			$percentileTimes[$percentile] = $times[ $index ];
		}

		return $percentileTimes;
	}
}
?>
