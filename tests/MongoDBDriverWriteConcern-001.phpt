--TEST--
MongoDB\Driver\WriteConcern construction
--FILE--
<?php
var_dump(new MongoDB\Driver\WriteConcern(MongoDB\Driver\WriteConcern::MAJORITY));
var_dump(new MongoDB\Driver\WriteConcern(MongoDB\Driver\WriteConcern::MAJORITY, 1000));

var_dump(new MongoDB\Driver\WriteConcern(2));
var_dump(new MongoDB\Driver\WriteConcern(2, 2000));

var_dump(new MongoDB\Driver\WriteConcern("tagname"));
var_dump(new MongoDB\Driver\WriteConcern("string", 3000));

var_dump(new MongoDB\Driver\WriteConcern("string", 4000, true));
var_dump(new MongoDB\Driver\WriteConcern("string", 5000, false));
?>
--EXPECTF--
object(MongoDB\Driver\WriteConcern)#%d (%d) {
  ["w"]=>
  string(8) "majority"
  ["wmajority"]=>
  bool(true)
  ["wtimeout"]=>
  int(0)
  ["journal"]=>
  NULL
}
object(MongoDB\Driver\WriteConcern)#%d (%d) {
  ["w"]=>
  string(8) "majority"
  ["wmajority"]=>
  bool(true)
  ["wtimeout"]=>
  int(1000)
  ["journal"]=>
  NULL
}
object(MongoDB\Driver\WriteConcern)#%d (%d) {
  ["w"]=>
  int(2)
  ["wmajority"]=>
  bool(false)
  ["wtimeout"]=>
  int(0)
  ["journal"]=>
  NULL
}
object(MongoDB\Driver\WriteConcern)#%d (%d) {
  ["w"]=>
  int(2)
  ["wmajority"]=>
  bool(false)
  ["wtimeout"]=>
  int(2000)
  ["journal"]=>
  NULL
}
object(MongoDB\Driver\WriteConcern)#%d (%d) {
  ["w"]=>
  string(7) "tagname"
  ["wmajority"]=>
  bool(false)
  ["wtimeout"]=>
  int(0)
  ["journal"]=>
  NULL
}
object(MongoDB\Driver\WriteConcern)#%d (%d) {
  ["w"]=>
  string(6) "string"
  ["wmajority"]=>
  bool(false)
  ["wtimeout"]=>
  int(3000)
  ["journal"]=>
  NULL
}
object(MongoDB\Driver\WriteConcern)#%d (%d) {
  ["w"]=>
  string(6) "string"
  ["wmajority"]=>
  bool(false)
  ["wtimeout"]=>
  int(4000)
  ["journal"]=>
  bool(true)
}
object(MongoDB\Driver\WriteConcern)#%d (%d) {
  ["w"]=>
  string(6) "string"
  ["wmajority"]=>
  bool(false)
  ["wtimeout"]=>
  int(5000)
  ["journal"]=>
  bool(false)
}
