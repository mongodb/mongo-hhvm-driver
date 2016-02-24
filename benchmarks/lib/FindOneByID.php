<?php
namespace MongoDB\Benchmark\Lib;
use \MongoDB\Benchmark\Base;
use \MongoDB\Benchmark\Task;

class FindOneByID extends Base implements Task
{
	function setup()
	{
		$this->m = new \MongoDB\Driver\Manager( 'mongodb://localhost' );

		// Drop perftest DB
		$cmd = new \MongoDB\Driver\Command( [ 'dropDatabase' => 1 ] );
		$this->m->executeCommand( 'perftest', $cmd );

		$this->data = json_decode( trim( file_get_contents( 'data/TWEET.json' ) ) );

		$bw = new \MongoDB\Driver\BulkWrite;
		for ( $i = 1; $i <= 10000; $i++ )
		{
			$newDoc = $this->data;
			$newDoc->_id = $i;

			$bw->insert( $newDoc );
		}
		try
		{
			$this->m->executeBulkWrite( 'perftest.corpus', $bw );
		}
		catch ( \Exception $e )
		{
			var_dump( $e );
		}

		$this->collection = (new \MongoDB\Client)->perftest->corpus;
	}

	function beforeTask()
	{
	}

	function doTask()
	{
		for ( $i = 1; $i <= 10000; $i++ )
		{
			$cursor = $this->collection->findOne( [ '_id' => $i ] );
			iterator_to_array( $cursor );
		}
	}

	function afterTask()
	{
	}

	function tearDown()
	{
		// Drop perftest DB
		$cmd = new \MongoDB\Driver\Command( [ 'dropDatabase' => 1 ] );
		$this->m->executeCommand( 'perftest', $cmd );
	}
}
?>
