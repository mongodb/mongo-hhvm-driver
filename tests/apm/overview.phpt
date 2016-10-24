--TEST--
HHVM-190: APM Specification
--FILE--
<?php
include dirname(__FILE__) . '/../utils.inc';

$m = new MongoDB\Driver\Manager("mongodb://localhost:27017");
cleanup( $m );

class MySubscriber implements MongoDB\Driver\Monitoring\CommandSubscriber
{
	public function commandStarted( \MongoDB\Driver\Monitoring\CommandStartedEvent $event )
	{
		echo "started:\n";
		var_dump( $event );
	}

	public function commandSucceeded( \MongoDB\Driver\Monitoring\CommandSucceededEvent $event )
	{
		echo "succeeded:\n";
		var_dump( $event );
	}

	public function commandFailed( \MongoDB\Driver\Monitoring\CommandFailedEvent $event )
	{
		echo "failed:\n";
		var_dump( $event );
	}
}

$m->addSubscriber( new MySubscriber() );
cleanup( $m );

$d = new \MongoDB\BSON\Decimal128("1234.5678");

$bw = new MongoDB\Driver\BulkWrite( [ 'ordered' => false ] );
$_id = $bw->insert( [ 'decimal' => $d ] );
$r = $m->executeBulkWrite( 'demo.test', $bw );

$query = new MongoDB\Driver\Query( [] );
$cursor = $m->executeQuery( "demo.test", $query );
var_dump( $cursor->toArray() );
?>
--EXPECTF--
started:
object(MongoDB\Driver\Monitoring\CommandStartedEvent)#%d (%d) {
  ["command":"MongoDB\Driver\Monitoring\CommandStartedEvent":private]=>
  object(stdClass)#%d (%d) {
    ["drop"]=>
    string(4) "test"
  }
  ["databaseName":"MongoDB\Driver\Monitoring\CommandStartedEvent":private]=>
  string(4) "demo"
  ["commandName":"MongoDB\Driver\Monitoring\_CommandEvent":private]=>
  string(4) "drop"
  ["server":"MongoDB\Driver\Monitoring\_CommandEvent":private]=>
  object(MongoDB\Driver\Server)#%d (%d) {
    ["host"]=>
    string(9) "localhost"
    ["port"]=>
    int(27017)
    %a
  }
  ["operationId":"MongoDB\Driver\Monitoring\_CommandEvent":private]=>
  int(%d)
  ["requestId":"MongoDB\Driver\Monitoring\_CommandEvent":private]=>
  int(%d)
}
failed:
object(MongoDB\Driver\Monitoring\CommandFailedEvent)#%d (%d) {
  ["error":"MongoDB\Driver\Monitoring\CommandFailedEvent":private]=>
  object(MongoDB\Driver\Exception\RuntimeException)#%d (%d) {
    ["message":protected]=>
    string(12) "ns not found"
    ["string":"Exception":private]=>
    string(0) ""
    ["code":protected]=>
    int(26)
    ["file":protected]=>
    string(%d) "%stests/utils.inc"
    ["line":protected]=>
    int(18)
    ["trace":"Exception":private]=>
    array(%d) {
      [0]=>
      array(%d) {
        ["file"]=>
        string(%d) "%stests/utils.inc"
        ["line"]=>
        int(18)
        ["function"]=>
        string(14) "executeCommand"
        ["class"]=>
        string(22) "MongoDB\Driver\Manager"
        ["type"]=>
        string(2) "->"
        ["args"]=>
        array(%d) {
          [0]=>
          string(4) "demo"
          [1]=>
          object(MongoDB\Driver\Command)#%d (%d) {
            ["command"]=>
            object(stdClass)#%d (%d) {
              ["drop"]=>
              string(4) "test"
            }
          }
        }
      }
      [1]=>
      array(%d) {
        ["file"]=>
        string(%d) "%stests/apm/overview.php"
        ["line"]=>
        int(29)
        ["function"]=>
        string(7) "cleanup"
        ["args"]=>
        array(%d) {
          [0]=>
          object(MongoDB\Driver\Manager)#%d (%d) {
            ["uri"]=>
            string(25) "mongodb://localhost:27017"
            ["cluster"]=>
            %a
          }
        }
      }
    }
    ["previous":"Exception":private]=>
    NULL
  }
  ["durationMicros":"MongoDB\Driver\Monitoring\_CommandResultEvent":private]=>
  int(%d)
  ["commandName":"MongoDB\Driver\Monitoring\_CommandEvent":private]=>
  string(4) "drop"
  ["server":"MongoDB\Driver\Monitoring\_CommandEvent":private]=>
  object(MongoDB\Driver\Server)#%d (%d) {
    ["host"]=>
    string(9) "localhost"
    ["port"]=>
    int(27017)
    %a
  }
  ["operationId":"MongoDB\Driver\Monitoring\_CommandEvent":private]=>
  int(%d)
  ["requestId":"MongoDB\Driver\Monitoring\_CommandEvent":private]=>
  int(%d)
}
started:
object(MongoDB\Driver\Monitoring\CommandStartedEvent)#%d (%d) {
  ["command":"MongoDB\Driver\Monitoring\CommandStartedEvent":private]=>
  object(stdClass)#%d (%d) {
    ["insert"]=>
    string(4) "test"
    ["writeConcern"]=>
    object(stdClass)#%d (%d) {
    }
    ["ordered"]=>
    bool(false)
    ["documents"]=>
    array(%d) {
      [0]=>
      object(stdClass)#%d (%d) {
        ["decimal"]=>
        object(MongoDB\BSON\Decimal128)#%d (%d) {
          ["dec"]=>
          string(9) "1234.5678"
        }
        ["_id"]=>
        object(MongoDB\BSON\ObjectID)#%d (%d) {
          ["oid"]=>
          string(24) "%s"
        }
      }
    }
  }
  ["databaseName":"MongoDB\Driver\Monitoring\CommandStartedEvent":private]=>
  string(4) "demo"
  ["commandName":"MongoDB\Driver\Monitoring\_CommandEvent":private]=>
  string(6) "insert"
  ["server":"MongoDB\Driver\Monitoring\_CommandEvent":private]=>
  object(MongoDB\Driver\Server)#%d (%d) {
    ["host"]=>
    string(9) "localhost"
    ["port"]=>
    int(27017)
    %a
  }
  ["operationId":"MongoDB\Driver\Monitoring\_CommandEvent":private]=>
  int(%d)
  ["requestId":"MongoDB\Driver\Monitoring\_CommandEvent":private]=>
  int(%d)
}
succeeded:
object(MongoDB\Driver\Monitoring\CommandSucceededEvent)#%d (%d) {
  ["reply":"MongoDB\Driver\Monitoring\CommandSucceededEvent":private]=>
  object(stdClass)#%d (%d) {
    ["n"]=>
    int(1)
    ["ok"]=>
    float(1)
  }
  ["durationMicros":"MongoDB\Driver\Monitoring\_CommandResultEvent":private]=>
  int(%d)
  ["commandName":"MongoDB\Driver\Monitoring\_CommandEvent":private]=>
  string(6) "insert"
  ["server":"MongoDB\Driver\Monitoring\_CommandEvent":private]=>
  object(MongoDB\Driver\Server)#%d (%d) {
    ["host"]=>
    string(9) "localhost"
    ["port"]=>
    int(27017)
    %a
  }
  ["operationId":"MongoDB\Driver\Monitoring\_CommandEvent":private]=>
  int(%d)
  ["requestId":"MongoDB\Driver\Monitoring\_CommandEvent":private]=>
  int(%d)
}
started:
object(MongoDB\Driver\Monitoring\CommandStartedEvent)#%d (%d) {
  ["command":"MongoDB\Driver\Monitoring\CommandStartedEvent":private]=>
  object(stdClass)#%d (%d) {
    ["find"]=>
    string(4) "test"
    ["filter"]=>
    object(stdClass)#%d (%d) {
    }
  }
  ["databaseName":"MongoDB\Driver\Monitoring\CommandStartedEvent":private]=>
  string(4) "demo"
  ["commandName":"MongoDB\Driver\Monitoring\_CommandEvent":private]=>
  string(4) "find"
  ["server":"MongoDB\Driver\Monitoring\_CommandEvent":private]=>
  object(MongoDB\Driver\Server)#%d (%d) {
    ["host"]=>
    string(9) "localhost"
    ["port"]=>
    int(27017)
    %a
  }
  ["operationId":"MongoDB\Driver\Monitoring\_CommandEvent":private]=>
  int(%d)
  ["requestId":"MongoDB\Driver\Monitoring\_CommandEvent":private]=>
  int(%d)
}
succeeded:
object(MongoDB\Driver\Monitoring\CommandSucceededEvent)#%d (%d) {
  ["reply":"MongoDB\Driver\Monitoring\CommandSucceededEvent":private]=>
  object(stdClass)#%d (%d) {
    ["cursor"]=>
    object(stdClass)#%d (%d) {
      ["firstBatch"]=>
      array(%d) {
        [0]=>
        object(stdClass)#%d (%d) {
          ["_id"]=>
          object(MongoDB\BSON\ObjectID)#%d (%d) {
            ["oid"]=>
            string(24) "%s"
          }
          ["decimal"]=>
          object(MongoDB\BSON\Decimal128)#%d (%d) {
            ["dec"]=>
            string(9) "1234.5678"
          }
        }
      }
      ["id"]=>
      int(0)
      ["ns"]=>
      string(9) "demo.test"
    }
    ["ok"]=>
    float(1)
  }
  ["durationMicros":"MongoDB\Driver\Monitoring\_CommandResultEvent":private]=>
  int(%d)
  ["commandName":"MongoDB\Driver\Monitoring\_CommandEvent":private]=>
  string(4) "find"
  ["server":"MongoDB\Driver\Monitoring\_CommandEvent":private]=>
  object(MongoDB\Driver\Server)#%d (%d) {
    ["host"]=>
    string(9) "localhost"
    ["port"]=>
    int(27017)
    %a
  }
  ["operationId":"MongoDB\Driver\Monitoring\_CommandEvent":private]=>
  int(%d)
  ["requestId":"MongoDB\Driver\Monitoring\_CommandEvent":private]=>
  int(%d)
}
array(%d) {
  [0]=>
  object(stdClass)#%d (%d) {
    ["_id"]=>
    object(MongoDB\BSON\ObjectID)#%d (%d) {
      ["oid"]=>
      string(24) "%s"
    }
    ["decimal"]=>
    object(MongoDB\BSON\Decimal128)#%d (%d) {
      ["dec"]=>
      string(9) "1234.5678"
    }
  }
}
