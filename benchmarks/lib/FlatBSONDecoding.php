<?php
namespace MongoDB\Benchmark\Lib;
use \MongoDB\Benchmark\BSONDecoding;
use \MongoDB\Benchmark\Task;

class FlatBSONDecoding extends BSONDecoding implements Task
{
	protected $data;

	function setup()
	{
		$this->data = $this->loadFile( "data/flat_bson.json" );	
	}

	function beforeTask()
	{
	}

	function doTask()
	{
		for ( $i = 0; $i < 10000; $i++ )
		{
			$decoded = \MongoDB\BSON\toPHP( $this->data );
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
