--TEST--
MongoDB\Driver\ReadPreference serialization with empty tagset
--FILE--
<?php
$data = [
	[ MongoDB\Driver\ReadPreference::RP_SECONDARY, [] ],
	[ MongoDB\Driver\ReadPreference::RP_SECONDARY, [ [] ] ],
	[ MongoDB\Driver\ReadPreference::RP_SECONDARY, [ [ 'foo' => 'bar' ], [] ] ],
];

foreach ($data as $item) {
	list($rp, $tags) = $item;
	$w = new MongoDB\Driver\ReadPreference($rp, $tags);
	var_dump( $w );
	echo "\n";
}
?>
--EXPECTF--
object(MongoDB\Driver\ReadPreference)#%d (1) {
  ["mode"]=>
  string(9) "secondary"
}

object(MongoDB\Driver\ReadPreference)#%d (2) {
  ["mode"]=>
  string(9) "secondary"
  ["tags"]=>
  array(1) {
    [0]=>
    object(stdClass)#%d (0) {
    }
  }
}

object(MongoDB\Driver\ReadPreference)#%d (2) {
  ["mode"]=>
  string(9) "secondary"
  ["tags"]=>
  array(2) {
    [0]=>
    object(stdClass)#%d (1) {
      ["foo"]=>
      string(3) "bar"
    }
    [1]=>
    object(stdClass)#%d (0) {
    }
  }
}
