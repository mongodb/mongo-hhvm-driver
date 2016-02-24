<?php
namespace MongoDB\Benchmark;

interface Task
{
	function setup();
	function beforeTask();
	function doTask();
	function afterTask();
	function teardown();
}
