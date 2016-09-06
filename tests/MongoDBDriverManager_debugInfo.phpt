--TEST--
MongoDB\Driver\Manager::__debugInfo()
--FILE--
<?php
$manager = new MongoDB\Driver\Manager();

$command = new MongoDB\Driver\Command(array('ping' => 1));
$manager->executeCommand("test", $command);

var_dump( $manager->__debugInfo() );
?>
--EXPECTF--
array(2) {
  ["uri"]=>
  string(%d) "%s"
  ["cluster"]=>
  array(1) {
    [0]=>
    array(10) {
      ["host"]=>
      string(%d) "%s"
      ["port"]=>
      int(%d)
      ["type"]=>
      int(1)
      ["is_primary"]=>
      bool(false)
      ["is_secondary"]=>
      bool(false)
      ["is_arbiter"]=>
      bool(false)
      ["is_hidden"]=>
      bool(false)
      ["is_passive"]=>
      bool(false)
      ["last_is_master"]=>
      array(%d) {
        ["ismaster"]=>
        bool(true)
        ["maxBsonObjectSize"]=>
        int(%d)
        ["maxMessageSizeBytes"]=>
        int(%d)
        ["maxWriteBatchSize"]=>
        int(%d)
        ["localTime"]=>
        object(MongoDB\BSON\UTCDateTime)#%d (1) {
          ["milliseconds"]=>
          string(%d) "%d"
        }
        ["maxWireVersion"]=>
        int(%d)
        ["minWireVersion"]=>
        int(%d)
        %a
        float(1)
      }
      ["round_trip_time"]=>
      int(%d)
    }
  }
}
