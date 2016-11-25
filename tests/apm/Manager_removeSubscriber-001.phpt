--TEST--
APM: Manager::removeSubscriber()
--SKIPIF--
skip APM temporarily removed
--FILE--
<?php
include dirname(__FILE__) . '/../utils.inc';

$m = new MongoDB\Driver\Manager("mongodb://localhost:27017");
cleanup( $m );

class MySubscriber implements MongoDB\Driver\Monitoring\CommandSubscriber
{
	public function commandStarted( \MongoDB\Driver\Monitoring\CommandStartedEvent $event )
	{
		echo "- started: ", $event->getCommandName(), "\n";
	}

	public function commandSucceeded( \MongoDB\Driver\Monitoring\CommandSucceededEvent $event )
	{
	}

	public function commandFailed( \MongoDB\Driver\Monitoring\CommandFailedEvent $event )
	{
	}
}

cleanup( $m );
$query = new MongoDB\Driver\Query( [] );
$subscriber = new MySubscriber;

echo "Before addSubscriber\n";
$cursor = $m->executeQuery( "demo.test", $query );

$m->addSubscriber( $subscriber );

echo "After addSubscriber\n";
$cursor = $m->executeQuery( "demo.test", $query );

$m->removeSubscriber( $subscriber );

echo "After removeSubscriber\n";
$cursor = $m->executeQuery( "demo.test", $query );
?>
--EXPECT--
Before addSubscriber
After addSubscriber
- started: find
After removeSubscriber
