--TEST--
MongoDB\Driver\Manager::getReadPreference() - URI only
--FILE--
<?php
$data = [
	'mongodb://localhost/?readPreference=primary',
	'mongodb://localhost/?readPreference=nearest&readPreferenceTags=dc:ny,use:ssd&readPreferenceTags=dc:sf&readPreferenceTags=',
];

foreach ($data as $item) {
	echo $item, "\n";
	$m = new MongoDB\Driver\Manager($item);
	var_dump($m->getReadPreference());
}
?>
--EXPECTF--
mongodb://localhost/?readPreference=primary
object(MongoDB\Driver\ReadPreference)#%d (%d) {
  ["mode"]=>
  string(7) "primary"
}
mongodb://localhost/?readPreference=nearest&readPreferenceTags=dc:ny,use:ssd&readPreferenceTags=dc:sf&readPreferenceTags=
object(MongoDB\Driver\ReadPreference)#%d (%d) {
  ["mode"]=>
  string(7) "nearest"
  ["tags"]=>
  array(3) {
    [0]=>
    array(2) {
      ["dc"]=>
      string(2) "ny"
      ["use"]=>
      string(3) "ssd"
    }
    [1]=>
    array(1) {
      ["dc"]=>
      string(2) "sf"
    }
    [2]=>
    array(0) {
    }
  }
}
