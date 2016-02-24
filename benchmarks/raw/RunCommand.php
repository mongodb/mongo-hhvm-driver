<?php
namespace MongoDB\Benchmark\Raw;
use \MongoDB\Benchmark\Base;
use \MongoDB\Benchmark\Task;

class RunCommand extends Base implements Task
{
	function setup()
	{
		$this->m = new \MongoDB\Driver\Manager( 'mongodb://localhost' );
	}

	function beforeTask()
	{
		$this->cmd = new \MongoDB\Driver\Command( [ 'ismaster' => true ] );
	}

	function doTask()
	{
		for ( $i = 0; $i < 10000; $i++ )
		{
			$this->m->executeCommand( 'test', $this->cmd );
		}
	}

	function afterTask()
	{
	}

	function tearDown()
	{
	}
}
?>
