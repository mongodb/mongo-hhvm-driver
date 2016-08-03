--TEST--
MongoDB\BSON\Regex value object
--FILE--
<?php
$tests = [
	[ "^foo" ],
	[ "^foo", "i" ],
	[ "^foo", "" ],
];

foreach ( $tests as $test )
{
	switch ( count( $test ) )
	{
		case 1:
			$d = new \MongoDB\BSON\Regex( $test[0] );
			break;
		case 2:
			$d = new \MongoDB\BSON\Regex( $test[0], $test[1] );
			break;
	}

	var_dump( $d );
	echo (string) $d, "\n\n";
}
?>
--EXPECTF--
object(MongoDB\BSON\Regex)#%d (%d) {
  ["pattern"]=>
  string(4) "^foo"
  ["flags"]=>
  string(0) ""
}
/^foo/

object(MongoDB\BSON\Regex)#%d (%d) {
  ["pattern"]=>
  string(4) "^foo"
  ["flags"]=>
  string(1) "i"
}
/^foo/i

object(MongoDB\BSON\Regex)#%d (%d) {
  ["pattern"]=>
  string(4) "^foo"
  ["flags"]=>
  string(0) ""
}
/^foo/
