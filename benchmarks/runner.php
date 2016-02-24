<?php
namespace MongoDB\Benchmark;

require 'vendor/autoload.php';

$iterations = 100;

require 'Task.php';
require 'Base.php';

require 'BSONEncoding.php';
require 'BSONDecoding.php';

require 'raw/FlatBSONEncoding.php';
require 'raw/DeepBSONEncoding.php';
require 'raw/FullBSONEncoding.php';
require 'raw/FlatBSONDecoding.php';
require 'raw/DeepBSONDecoding.php';
require 'raw/FullBSONDecoding.php';
require 'raw/RunCommand.php';
require 'raw/FindOneByID.php';
require 'raw/InsertOneSmallDoc.php';
require 'raw/InsertOneLargeDoc.php';

require 'lib/FlatBSONEncoding.php';
require 'lib/DeepBSONEncoding.php';
require 'lib/FullBSONEncoding.php';
require 'lib/FlatBSONDecoding.php';
require 'lib/DeepBSONDecoding.php';
require 'lib/FullBSONDecoding.php';
require 'lib/RunCommand.php';
require 'lib/FindOneByID.php';
require 'lib/InsertOneSmallDoc.php';
require 'lib/InsertOneLargeDoc.php';

$taskClasses = [
//	'\MongoDB\Benchmark\Raw\FlatBSONEncoding',
//	'\MongoDB\Benchmark\Raw\DeepBSONEncoding',
//	'\MongoDB\Benchmark\Raw\FullBSONEncoding',
//	'\MongoDB\Benchmark\Raw\FlatBSONDecoding',
//	'\MongoDB\Benchmark\Raw\DeepBSONDecoding',
//	'\MongoDB\Benchmark\Raw\FullBSONDecoding',
//	'\MongoDB\Benchmark\Raw\RunCommand',
	'\MongoDB\Benchmark\Raw\FindOneByID',
	'\MongoDB\Benchmark\Raw\InsertOneSmallDoc',
	'\MongoDB\Benchmark\Raw\InsertOneLargeDoc',
//
//	'\MongoDB\Benchmark\Lib\FlatBSONEncoding',
//	'\MongoDB\Benchmark\Lib\DeepBSONEncoding',
//	'\MongoDB\Benchmark\Lib\FullBSONEncoding',
//	'\MongoDB\Benchmark\Lib\FlatBSONDecoding',
//	'\MongoDB\Benchmark\Lib\DeepBSONDecoding',
//	'\MongoDB\Benchmark\Lib\FullBSONDecoding',
//	'\MongoDB\Benchmark\Lib\RunCommand',
	'\MongoDB\Benchmark\Lib\FindOneByID',
	'\MongoDB\Benchmark\Lib\InsertOneSmallDoc',
	'\MongoDB\Benchmark\Lib\InsertOneLargeDoc',
];

for ($j = 0; $j < 1; $j++)
{
	echo "RUN $j\n";

foreach ( $taskClasses as $taskClass )
{
	$times = [];
	$task = new $taskClass;

	$task->setup();

	for ( $i = 0; $i < $iterations; $i++ )
	{
		$task->beforeTask();
		
		$before = microtime( true );
		$task->doTask();
		$after = microtime( true );

		$times[] = $after - $before;

		$task->afterTask();
		echo ".";
	}
	echo "\n";

	$task->teardown();

	$percentiles = $task->calculatePercentiles( $times );
	echo $taskClass, ': ', $percentiles[50], "\n";
}

}
?>
