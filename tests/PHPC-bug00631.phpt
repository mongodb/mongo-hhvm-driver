--TEST--
PHPC-631: UTCDateTime::toDateTime() may return object that cannot be serialized
--SKIPIF--
<?php if (!defined("HHVM_VERSION_ID")) exit("skip PHPC uses '+00:00' instead of 'UTC'"); ?>
--INI--
date.timezone=UTC
--FILE--
<?php
$utcdatetime = new MongoDB\BSON\UTCDateTime('1466540755123');
$datetime = $utcdatetime->toDateTime();
$s = serialize($datetime);

var_dump($datetime);

echo "\n", $s, "\n\n";

var_dump(unserialize($s));

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
object(DateTime)#%d (%d) {
  ["date"]=>
  string(26) "2016-06-21 20:25:55.123000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}

O:8:"DateTime":3:{s:4:"date";s:26:"2016-06-21 20:25:55.123000";s:13:"timezone_type";i:3;s:8:"timezone";s:3:"UTC";}

object(DateTime)#%d (%d) {
  ["date"]=>
  string(26) "2016-06-21 20:25:55.123000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
===DONE===
