--TEST--
MongoDB\Driver\Manager::getReadPreference()
--FILE--
<?php
$data = [
	'mongodb://localhost/?w=1&journal=true',
	'mongodb://localhost/?w=majority&wtimeoutms=500&fsync=false',
	'mongodb://localhost/?w=dc1&fsync=true&journal=true',
];

foreach ($data as $item) {
	echo $item, "\n";
	$m = new MongoDB\Driver\Manager($item);
	var_dump($m->getWriteConcern());
}
?>
--EXPECTF--
mongodb://localhost/?w=1&journal=true
object(MongoDB\Driver\WriteConcern)#%d (5) {
  ["w"]=>
  int(1)
  ["wmajority"]=>
  bool(false)
  ["wtimeout"]=>
  int(0)
  ["fsync"]=>
  NULL
  ["journal"]=>
  bool(true)
}
mongodb://localhost/?w=majority&wtimeoutms=500&fsync=false
object(MongoDB\Driver\WriteConcern)#%d (5) {
  ["w"]=>
  string(8) "majority"
  ["wmajority"]=>
  bool(true)
  ["wtimeout"]=>
  int(500)
  ["fsync"]=>
  NULL
  ["journal"]=>
  NULL
}
mongodb://localhost/?w=dc1&fsync=true&journal=true
object(MongoDB\Driver\WriteConcern)#%d (5) {
  ["w"]=>
  string(3) "dc1"
  ["wmajority"]=>
  bool(false)
  ["wtimeout"]=>
  int(0)
  ["fsync"]=>
  NULL
  ["journal"]=>
  bool(true)
}
