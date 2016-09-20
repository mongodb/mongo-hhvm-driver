--TEST--
MongoDB\Driver\Manager::getReadPreference() - with options array
--FILE--
<?php
$data = [
	[
		'mongodb://localhost/?readPreference=secondary',
		[ 'readPreference' => 'secondaryPreferred' ]
	],

	[
		'mongodb://localhost/?readPreference=secondary',
		[ 'readPreferenceTags' => [] ]
	],

	[
		'mongodb://localhost/?readPreference=secondary&readPreferenceTags=dc:ny,use:reports&readPreferenceTags=',
		[]
	],

	[
		'mongodb://localhost/?readPreference=secondary',
		[
			'readPreferenceTags' => [
				[ 'dc' => 'ny', 'use' => 'reports' ],
				[ ]
			],
		]
	],

	[
		'mongodb://localhost/?readPreference=secondary&readPreferenceTags=dc:ny,use:reports',
		[
			'readPreferenceTags' => [
				[ 'dc' => 'ca' ]
			]
		]
	],
];

foreach ($data as $item) {
	echo $item[0], "\n";
	echo str_replace("\n", "", var_export( $item[1], true ) ) . ":\n";
	$m = new MongoDB\Driver\Manager( $item[0], $item[1] );
	var_dump($m->getReadPreference());
	echo "\n";
}
?>
--EXPECTF--
mongodb://localhost/?readPreference=secondary
array (  'readPreference' => 'secondaryPreferred',):
object(MongoDB\Driver\ReadPreference)#%d (1) {
  ["mode"]=>
  string(18) "secondaryPreferred"
}

mongodb://localhost/?readPreference=secondary
array (  'readPreferenceTags' =>   array (  ),):
object(MongoDB\Driver\ReadPreference)#3 (1) {
  ["mode"]=>
  string(9) "secondary"
}

mongodb://localhost/?readPreference=secondary&readPreferenceTags=dc:ny,use:reports&readPreferenceTags=
array ():
object(MongoDB\Driver\ReadPreference)#%d (2) {
  ["mode"]=>
  string(9) "secondary"
  ["tags"]=>
  array(2) {
    [0]=>
    array(2) {
      ["dc"]=>
      string(2) "ny"
      ["use"]=>
      string(7) "reports"
    }
    [1]=>
    array(0) {
    }
  }
}

mongodb://localhost/?readPreference=secondary
array (  'readPreferenceTags' =>   array (    0 =>     array (      'dc' => 'ny',      'use' => 'reports',    ),    1 =>     array (    ),  ),):
object(MongoDB\Driver\ReadPreference)#%d (2) {
  ["mode"]=>
  string(9) "secondary"
  ["tags"]=>
  array(2) {
    [0]=>
    array(2) {
      ["dc"]=>
      string(2) "ny"
      ["use"]=>
      string(7) "reports"
    }
    [1]=>
    array(0) {
    }
  }
}

mongodb://localhost/?readPreference=secondary&readPreferenceTags=dc:ny,use:reports
array (  'readPreferenceTags' =>   array (    0 =>     array (      'dc' => 'ca',    ),  ),):
object(MongoDB\Driver\ReadPreference)#%d (2) {
  ["mode"]=>
  string(9) "secondary"
  ["tags"]=>
  array(1) {
    [0]=>
    array(1) {
      ["dc"]=>
      string(2) "ca"
    }
  }
}

