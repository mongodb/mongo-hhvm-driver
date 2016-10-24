--TEST--
APM: commandSucceeded callback (requestId and operationId match)
--FILE--
<?php
include dirname(__FILE__) . '/../utils.inc';

$m = new MongoDB\Driver\Manager("mongodb://localhost:27017");
cleanup( $m );

class MySubscriber implements MongoDB\Driver\Monitoring\CommandSubscriber
{
	public function commandStarted( \MongoDB\Driver\Monitoring\CommandStartedEvent $event )
	{
		echo "started: ", $event->getCommandName(), "\n";
		$this->startRequestId = $event->getRequestId();
		$this->startOperationId = $event->getOperationId();
	}

	public function commandSucceeded( \MongoDB\Driver\Monitoring\CommandSucceededEvent $event )
	{
		echo "succeeded: ", $event->getCommandName(), "\n";
		echo "- requestId matches: ", $this->startRequestId == $event->getRequestId() ? 'yes' : 'no', " \n";
		echo "- operationId matches: ", $this->startOperationId == $event->getOperationId() ? 'yes' : 'no', " \n";
	}

	public function commandFailed( \MongoDB\Driver\Monitoring\CommandFailedEvent $event )
	{
	}
}

$query = new MongoDB\Driver\Query( [] );
$subscriber = new MySubscriber;

$m->addSubscriber( $subscriber );

$cursor = $m->executeQuery( "demo.test", $query );
?>
--EXPECT--
started: find
succeeded: find
- requestId matches: yes 
- operationId matches: yes
