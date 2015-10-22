--TEST--
MongoDB\Driver\Cursor::setTypeMap()
--FILE--
<?php
$m = new MongoDB\Driver\Manager("mongodb://localhost:27017");

$c = new MongoDB\Driver\Command( [ 'drop' => 'test'] );
try {
	$m->executeCommand( 'demo', $c );
}
catch ( MongoDB\Driver\Exception\RuntimeException $e )
{
	// Ignore "ns not found" errors
	if ( $e->getCode() == 59 ) {
		throw $e;
	}
}

$map = new StdClass;
$map->bar = 52;
$map->foo = true;

$doc = [
	'array' => [ 1, 5, 'foo' ],
	'document' => $map,
];

$bw = new MongoDB\Driver\BulkWrite;
$bw->insert( $doc );
$r = $m->executeBulkWrite( 'demo.test', $bw );

$q = new MongoDB\Driver\Query( [] );

$typemaps = [
	[ ],
	[ 'array' => 'array' ],
	[ 'document' => 'object' ],
	[ 'document' => 'stdClass' ],
	[ 'array' => 'object' ],
	[ 'array' => 'stdClass' ],
	[ 'document' => 'array' ],
	[ 'root' => 'array' ],
	[ 'array' => 'object', 'document' => 'object' ],
	[ 'array' => 'array', 'document' => 'object' ],
	[ 'array' => 'object', 'document' => 'array' ],
	[ 'array' => 'array', 'document' => 'array' ],
	[ 'root' => 'array', 'array' => 'object', 'document' => 'object' ],
	[ 'root' => 'array', 'array' => 'array', 'document' => 'object' ],
	[ 'root' => 'array', 'array' => 'object', 'document' => 'array' ],
	[ 'root' => 'array', 'array' => 'array', 'document' => 'array' ],
	[ 'root' => 'object', 'array' => 'object', 'document' => 'object' ],
	[ 'root' => 'object', 'array' => 'array', 'document' => 'object' ],
	[ 'root' => 'object', 'array' => 'object', 'document' => 'array' ],
	[ 'root' => 'object', 'array' => 'array', 'document' => 'array' ],
];

foreach ( $typemaps as $typemap )
{
	foreach ( $typemap as $k => $v )
	{
		echo $k, ': ', $v, '  ';
	}
	echo "\n";

	$cursor = $m->executeQuery( 'demo.test', $q );
	$cursor->setTypeMap( $typemap );
	foreach ( $cursor as $key => $result )
	{
		var_dump( $result );
	}

	echo "\n=================\n\n";
}
?>
--EXPECTF--
object(stdClass)#%d (3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectID)#%d (1) {
    ["oid"]=>
    string(24) "%s"
  }
  ["array"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(5)
    [2]=>
    string(3) "foo"
  }
  ["document"]=>
  object(stdClass)#%d (2) {
    ["bar"]=>
    int(52)
    ["foo"]=>
    bool(true)
  }
}

=================

array: array  
object(stdClass)#%d (3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectID)#%d (1) {
    ["oid"]=>
    string(24) "%s"
  }
  ["array"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(5)
    [2]=>
    string(3) "foo"
  }
  ["document"]=>
  object(stdClass)#%d (2) {
    ["bar"]=>
    int(52)
    ["foo"]=>
    bool(true)
  }
}

=================

document: object  
object(stdClass)#%d (3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectID)#%d (1) {
    ["oid"]=>
    string(24) "%s"
  }
  ["array"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(5)
    [2]=>
    string(3) "foo"
  }
  ["document"]=>
  object(stdClass)#%d (2) {
    ["bar"]=>
    int(52)
    ["foo"]=>
    bool(true)
  }
}

=================

document: stdClass  
object(stdClass)#%d (3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectID)#%d (1) {
    ["oid"]=>
    string(24) "%s"
  }
  ["array"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(5)
    [2]=>
    string(3) "foo"
  }
  ["document"]=>
  object(stdClass)#%d (2) {
    ["bar"]=>
    int(52)
    ["foo"]=>
    bool(true)
  }
}

=================

array: object  
object(stdClass)#%d (3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectID)#%d (1) {
    ["oid"]=>
    string(24) "%s"
  }
  ["array"]=>
  object(stdClass)#%d (3) {
    [0]=>
    int(1)
    [1]=>
    int(5)
    [2]=>
    string(3) "foo"
  }
  ["document"]=>
  object(stdClass)#%d (2) {
    ["bar"]=>
    int(52)
    ["foo"]=>
    bool(true)
  }
}

=================

array: stdClass  
object(stdClass)#%d (3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectID)#%d (1) {
    ["oid"]=>
    string(24) "%s"
  }
  ["array"]=>
  object(stdClass)#%d (3) {
    [0]=>
    int(1)
    [1]=>
    int(5)
    [2]=>
    string(3) "foo"
  }
  ["document"]=>
  object(stdClass)#%d (2) {
    ["bar"]=>
    int(52)
    ["foo"]=>
    bool(true)
  }
}

=================

document: array  
object(stdClass)#%d (3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectID)#%d (1) {
    ["oid"]=>
    string(24) "%s"
  }
  ["array"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(5)
    [2]=>
    string(3) "foo"
  }
  ["document"]=>
  array(2) {
    ["bar"]=>
    int(52)
    ["foo"]=>
    bool(true)
  }
}

=================

root: array  
array(3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectID)#%d (1) {
    ["oid"]=>
    string(24) "%s"
  }
  ["array"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(5)
    [2]=>
    string(3) "foo"
  }
  ["document"]=>
  object(stdClass)#%d (2) {
    ["bar"]=>
    int(52)
    ["foo"]=>
    bool(true)
  }
}

=================

array: object  document: object  
object(stdClass)#%d (3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectID)#%d (1) {
    ["oid"]=>
    string(24) "%s"
  }
  ["array"]=>
  object(stdClass)#%d (3) {
    [0]=>
    int(1)
    [1]=>
    int(5)
    [2]=>
    string(3) "foo"
  }
  ["document"]=>
  object(stdClass)#%d (2) {
    ["bar"]=>
    int(52)
    ["foo"]=>
    bool(true)
  }
}

=================

array: array  document: object  
object(stdClass)#%d (3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectID)#%d (1) {
    ["oid"]=>
    string(24) "%s"
  }
  ["array"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(5)
    [2]=>
    string(3) "foo"
  }
  ["document"]=>
  object(stdClass)#%d (2) {
    ["bar"]=>
    int(52)
    ["foo"]=>
    bool(true)
  }
}

=================

array: object  document: array  
object(stdClass)#%d (3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectID)#%d (1) {
    ["oid"]=>
    string(24) "%s"
  }
  ["array"]=>
  object(stdClass)#%d (3) {
    [0]=>
    int(1)
    [1]=>
    int(5)
    [2]=>
    string(3) "foo"
  }
  ["document"]=>
  array(2) {
    ["bar"]=>
    int(52)
    ["foo"]=>
    bool(true)
  }
}

=================

array: array  document: array  
object(stdClass)#%d (3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectID)#%d (1) {
    ["oid"]=>
    string(24) "%s"
  }
  ["array"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(5)
    [2]=>
    string(3) "foo"
  }
  ["document"]=>
  array(2) {
    ["bar"]=>
    int(52)
    ["foo"]=>
    bool(true)
  }
}

=================

root: array  array: object  document: object  
array(3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectID)#%d (1) {
    ["oid"]=>
    string(24) "%s"
  }
  ["array"]=>
  object(stdClass)#%d (3) {
    [0]=>
    int(1)
    [1]=>
    int(5)
    [2]=>
    string(3) "foo"
  }
  ["document"]=>
  object(stdClass)#%d (2) {
    ["bar"]=>
    int(52)
    ["foo"]=>
    bool(true)
  }
}

=================

root: array  array: array  document: object  
array(3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectID)#%d (1) {
    ["oid"]=>
    string(24) "%s"
  }
  ["array"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(5)
    [2]=>
    string(3) "foo"
  }
  ["document"]=>
  object(stdClass)#%d (2) {
    ["bar"]=>
    int(52)
    ["foo"]=>
    bool(true)
  }
}

=================

root: array  array: object  document: array  
array(3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectID)#%d (1) {
    ["oid"]=>
    string(24) "%s"
  }
  ["array"]=>
  object(stdClass)#%d (3) {
    [0]=>
    int(1)
    [1]=>
    int(5)
    [2]=>
    string(3) "foo"
  }
  ["document"]=>
  array(2) {
    ["bar"]=>
    int(52)
    ["foo"]=>
    bool(true)
  }
}

=================

root: array  array: array  document: array  
array(3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectID)#%d (1) {
    ["oid"]=>
    string(24) "%s"
  }
  ["array"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(5)
    [2]=>
    string(3) "foo"
  }
  ["document"]=>
  array(2) {
    ["bar"]=>
    int(52)
    ["foo"]=>
    bool(true)
  }
}

=================

root: object  array: object  document: object  
object(stdClass)#%d (3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectID)#%d (1) {
    ["oid"]=>
    string(24) "%s"
  }
  ["array"]=>
  object(stdClass)#%d (3) {
    [0]=>
    int(1)
    [1]=>
    int(5)
    [2]=>
    string(3) "foo"
  }
  ["document"]=>
  object(stdClass)#%d (2) {
    ["bar"]=>
    int(52)
    ["foo"]=>
    bool(true)
  }
}

=================

root: object  array: array  document: object  
object(stdClass)#%d (3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectID)#%d (1) {
    ["oid"]=>
    string(24) "%s"
  }
  ["array"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(5)
    [2]=>
    string(3) "foo"
  }
  ["document"]=>
  object(stdClass)#%d (2) {
    ["bar"]=>
    int(52)
    ["foo"]=>
    bool(true)
  }
}

=================

root: object  array: object  document: array  
object(stdClass)#%d (3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectID)#%d (1) {
    ["oid"]=>
    string(24) "%s"
  }
  ["array"]=>
  object(stdClass)#%d (3) {
    [0]=>
    int(1)
    [1]=>
    int(5)
    [2]=>
    string(3) "foo"
  }
  ["document"]=>
  array(2) {
    ["bar"]=>
    int(52)
    ["foo"]=>
    bool(true)
  }
}

=================

root: object  array: array  document: array  
object(stdClass)#%d (3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectID)#%d (1) {
    ["oid"]=>
    string(24) "%s"
  }
  ["array"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(5)
    [2]=>
    string(3) "foo"
  }
  ["document"]=>
  array(2) {
    ["bar"]=>
    int(52)
    ["foo"]=>
    bool(true)
  }
}

=================
