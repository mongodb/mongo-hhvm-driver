--TEST--
APM: Manager::addSubscriber() (multiple)
--FILE--
<?php
include dirname(__FILE__) . '/../utils.inc';

$m = new MongoDB\Driver\Manager("mongodb://localhost:27017");
cleanup( $m );

class MySubscriber implements MongoDB\Driver\Monitoring\CommandSubscriber
{
	private $instanceName;

	public function __construct( $instanceName )
	{
		$this->instanceName = $instanceName;
	}

	public function commandStarted( \MongoDB\Driver\Monitoring\CommandStartedEvent $event )
	{
		echo "- ({$this->instanceName}) - started: ", $event->getCommandName(), "\n";
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
$subscriber1 = new MySubscriber( "ONE" );
$subscriber2 = new MySubscriber( "TWO" );

echo "Before addSubscriber\n";
$cursor = $m->executeQuery( "demo.test", $query );

MongoDB\Monitoring\addSubscriber( $subscriber1 );

echo "After addSubscriber (ONE)\n";
$cursor = $m->executeQuery( "demo.test", $query );

MongoDB\Monitoring\addSubscriber( $subscriber2 );

echo "After addSubscriber (TWO)\n";
$cursor = $m->executeQuery( "demo.test", $query );
?>
--EXPECT--
Before addSubscriber
After addSubscriber (ONE)
- (ONE) - started: find
After addSubscriber (TWO)
- (ONE) - started: find
- (TWO) - started: find
