<?php
namespace MongoDB\Benchmark\Raw;
use \MongoDB\Benchmark\Base;
use \MongoDB\Benchmark\Task;

class InsertOneLargeDoc extends Base implements Task
{
	function setup()
	{
		$this->m = new \MongoDB\Driver\Manager( 'mongodb://localhost' );

		// Drop perftest DB
		$cmd = new \MongoDB\Driver\Command( [ 'dropDatabase' => 1 ] );
		$this->m->executeCommand( 'perftest', $cmd );

		$this->data = json_decode( trim( file_get_contents( 'data/LARGE_DOC.json' ) ) );
	}

	function beforeTask()
	{
		/* Drop the corpus collection */
		$cmd = new \MongoDB\Driver\Command( [ 'drop' => 'corpus' ] );
		try
		{
			$this->m->executeCommand( 'perftest', $cmd );
		}
		catch ( \Exception $e )
		{
		}
		
		/* Create the corpus collection */
		$cmd = new \MongoDB\Driver\Command( [ 'create' => 'corpus' ] );
		$this->m->executeCommand( 'perftest', $cmd );
	}

	function doTask()
	{
		for ( $i = 1; $i <= 10; $i++ )
		{
			$bw = new \MongoDB\Driver\BulkWrite;
			$bw->insert( $this->data );

			$this->m->executeBulkWrite( 'perftest.corpus', $bw );
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
