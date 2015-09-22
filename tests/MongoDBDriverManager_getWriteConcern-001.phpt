--TEST--
MongoDB\Driver\Manager::getWriteConcern()
--FILE--
<?php
$data = [
	'mongodb://localhost/?w=1&journal=true',
	'mongodb://localhost/?w=majority&wtimeoutms=500',
	'mongodb://localhost/?w=dc1&journal=true',
];

foreach ($data as $item) {
	echo $item, "\n";
	$m = new MongoDB\Driver\Manager($item);
	var_dump($m->getWriteConcern());
}
?>
--EXPECTF--
mongodb://localhost/?w=1&journal=true
object(MongoDB\Driver\WriteConcern)#%d (4) {
  ["w"]=>
  int(1)
  ["wmajority"]=>
  bool(false)
  ["wtimeout"]=>
  int(0)
  ["journal"]=>
  bool(true)
}
mongodb://localhost/?w=majority&wtimeoutms=500
object(MongoDB\Driver\WriteConcern)#%d (4) {
  ["w"]=>
  string(8) "majority"
  ["wmajority"]=>
  bool(true)
  ["wtimeout"]=>
  int(500)
  ["journal"]=>
  NULL
}
mongodb://localhost/?w=dc1&journal=true
object(MongoDB\Driver\WriteConcern)#%d (4) {
  ["w"]=>
  string(3) "dc1"
  ["wmajority"]=>
  bool(false)
  ["wtimeout"]=>
  int(0)
  ["journal"]=>
  bool(true)
}
