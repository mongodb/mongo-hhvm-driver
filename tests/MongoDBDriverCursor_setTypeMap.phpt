--TEST--
MongoDB\Driver\Cursor::setTypeMap()
--FILE--
<?php
$m = new MongoDB\Driver\Manager("mongodb://localhost:27017");

$c = new MongoDB\Driver\Command( [ 'drop' => 'test'] );
try {
	$cursor = $m->executeCommand( 'demo', $c );

	foreach( $cursor->toArray() as $result )
	{
		var_dump($result);
	}
}
catch ( InvalidArgumentException $e )
{
	echo $e->getMessage(), "\n";
}

echo "================\n";

$map = new StdClass;
$map->bar = 52;
$map->foo = true;

$doc = [
	'array' => [ 1, 5, 'foo' ],
	'document' => $map,
];

$r = $m->executeInsert( 'demo.test', $doc );

$q = new MongoDB\Driver\Query( [] );

$typemaps = [
	[ ],
	[ 'array' => 'array' ],
	[ 'document' => 'object' ],
	[ 'document' => 'stdClass' ],
	[ 'array' => 'object' ],
	[ 'array' => 'stdClass' ],
	[ 'document' => 'array' ],
	[ 'array' => 'object', 'document' => 'object' ],
	[ 'array' => 'array', 'document' => 'object' ],
	[ 'array' => 'object', 'document' => 'array' ],
	[ 'array' => 'array', 'document' => 'array' ],
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
--EXPECT--
object(stdClass)#4 (3) {
  ["ns"]=>
  string(9) "demo.test"
  ["nIndexesWas"]=>
  int(1)
  ["ok"]=>
  float(1)
}
================

object(stdClass)#13 (3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectId)#10 (0) {
  }
  ["array"]=>
  object(stdClass)#11 (3) {
    [0]=>
    int(1)
    [1]=>
    int(5)
    [2]=>
    string(3) "foo"
  }
  ["document"]=>
  object(stdClass)#12 (2) {
    ["bar"]=>
    int(52)
    ["foo"]=>
    bool(true)
  }
}

=================

array: array  
object(stdClass)#18 (3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectId)#15 (0) {
  }
  ["array"]=>
  object(stdClass)#16 (3) {
    [0]=>
    int(1)
    [1]=>
    int(5)
    [2]=>
    string(3) "foo"
  }
  ["document"]=>
  object(stdClass)#17 (2) {
    ["bar"]=>
    int(52)
    ["foo"]=>
    bool(true)
  }
}

=================

document: object  
object(stdClass)#23 (3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectId)#20 (0) {
  }
  ["array"]=>
  object(stdClass)#21 (3) {
    [0]=>
    int(1)
    [1]=>
    int(5)
    [2]=>
    string(3) "foo"
  }
  ["document"]=>
  object(stdClass)#22 (2) {
    ["bar"]=>
    int(52)
    ["foo"]=>
    bool(true)
  }
}

=================

document: stdClass  
object(stdClass)#28 (3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectId)#25 (0) {
  }
  ["array"]=>
  object(stdClass)#26 (3) {
    [0]=>
    int(1)
    [1]=>
    int(5)
    [2]=>
    string(3) "foo"
  }
  ["document"]=>
  object(stdClass)#27 (2) {
    ["bar"]=>
    int(52)
    ["foo"]=>
    bool(true)
  }
}

=================

array: object  
object(stdClass)#33 (3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectId)#30 (0) {
  }
  ["array"]=>
  object(stdClass)#31 (3) {
    [0]=>
    int(1)
    [1]=>
    int(5)
    [2]=>
    string(3) "foo"
  }
  ["document"]=>
  object(stdClass)#32 (2) {
    ["bar"]=>
    int(52)
    ["foo"]=>
    bool(true)
  }
}

=================

array: stdClass  
object(stdClass)#38 (3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectId)#35 (0) {
  }
  ["array"]=>
  object(stdClass)#36 (3) {
    [0]=>
    int(1)
    [1]=>
    int(5)
    [2]=>
    string(3) "foo"
  }
  ["document"]=>
  object(stdClass)#37 (2) {
    ["bar"]=>
    int(52)
    ["foo"]=>
    bool(true)
  }
}

=================

document: array  
array(3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectId)#40 (0) {
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

array: object  document: object  
object(stdClass)#45 (3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectId)#42 (0) {
  }
  ["array"]=>
  object(stdClass)#43 (3) {
    [0]=>
    int(1)
    [1]=>
    int(5)
    [2]=>
    string(3) "foo"
  }
  ["document"]=>
  object(stdClass)#44 (2) {
    ["bar"]=>
    int(52)
    ["foo"]=>
    bool(true)
  }
}

=================

array: array  document: object  
object(stdClass)#50 (3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectId)#47 (0) {
  }
  ["array"]=>
  object(stdClass)#48 (3) {
    [0]=>
    int(1)
    [1]=>
    int(5)
    [2]=>
    string(3) "foo"
  }
  ["document"]=>
  object(stdClass)#49 (2) {
    ["bar"]=>
    int(52)
    ["foo"]=>
    bool(true)
  }
}

=================

array: object  document: array  
array(3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectId)#52 (0) {
  }
  ["array"]=>
  object(stdClass)#53 (3) {
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
array(3) {
  ["_id"]=>
  object(MongoDB\BSON\ObjectId)#55 (0) {
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
