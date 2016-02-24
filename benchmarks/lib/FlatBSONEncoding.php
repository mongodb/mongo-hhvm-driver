<?php
namespace MongoDB\Benchmark\Lib;
use \MongoDB\Benchmark\BSONEncoding;
use \MongoDB\Benchmark\Task;

class FlatBSONEncoding extends BSONEncoding implements Task
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
			$encoded = \MongoDB\BSON\fromPHP( $this->data );
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
