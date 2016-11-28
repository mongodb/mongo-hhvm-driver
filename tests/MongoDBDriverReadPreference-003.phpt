--TEST--
MongoDB\Driver\ReadPreference construction (primary with default maxStalenessSeconds)
--FILE--
<?php
require_once __DIR__ . '/utils.inc';

$rp = new MongoDB\Driver\ReadPreference(MongoDB\Driver\ReadPreference::RP_PRIMARY, null, ['maxStalenessSeconds' => -1]);
var_dump( $rp );
?>
--EXPECT--
object(MongoDB\Driver\ReadPreference)#%d (%d) {
  ["mode"]=>
  string(7) "primary"
}
