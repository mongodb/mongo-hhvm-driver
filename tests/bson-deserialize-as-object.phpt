--TEST--
BSON deserialization: unserialize as object
--FILE--
<?php
$tests = [
	'[ 42, 43, 44 ]', // a top-level can't be an array, but we test for it anyway
	'{ "breakfast" : "cheese", "dinner" : "sausages" }',
	'{ "nested" : [ "foo", "bar", "baz" ] }',
	'{ "nested" : { "breakfast" : "cheese", "dinner" : "sausages" } }',
	'{ "__pclass": { "$type": "80", "$binary" : "TGlxdWlkQ2xhc3M=" }, "data" : [ "foo", "bar", "baz" ] }',
	'{ "data" : { "__pclass" : { "$type": "80", "$binary" : "TGlxdWlkQ2xhc3M=" }, "breakfast" : "cheese", "dinner" : "sausages" } }',
	'{ "__pclass": { "$type": "80", "$binary" : "TGlxdWlkQ2xhc3M=" }, "data" : { "__pclass": { "$type": "80", "$binary" : "TGlxdWlkQ2xhc3M=" }, "breakfast" : "cheese", "dinner" : "sausages" } }',
];

$typemap = [ 'array' => 'object', 'document' => 'object', 'root' => 'object' ];

foreach ( $tests as $test )
{
	echo $test, "\n";
	$bson = MongoDB\BSON\fromJSON( $test );
	var_dump( MongoDB\BSON\toPHP( $bson, $typemap ) );
	echo "\n";
}
?>
--EXPECTF--
[ 42, 43, 44 ]
object(stdClass)#%d (3) {
  [0]=>
  int(42)
  [1]=>
  int(43)
  [2]=>
  int(44)
}

{ "breakfast" : "cheese", "dinner" : "sausages" }
object(stdClass)#%d (2) {
  ["breakfast"]=>
  string(6) "cheese"
  ["dinner"]=>
  string(8) "sausages"
}

{ "nested" : [ "foo", "bar", "baz" ] }
object(stdClass)#%d (1) {
  ["nested"]=>
  object(stdClass)#%d (3) {
    [0]=>
    string(3) "foo"
    [1]=>
    string(3) "bar"
    [2]=>
    string(3) "baz"
  }
}

{ "nested" : { "breakfast" : "cheese", "dinner" : "sausages" } }
object(stdClass)#%d (1) {
  ["nested"]=>
  object(stdClass)#%d (2) {
    ["breakfast"]=>
    string(6) "cheese"
    ["dinner"]=>
    string(8) "sausages"
  }
}

{ "__pclass": { "$type": "80", "$binary" : "TGlxdWlkQ2xhc3M=" }, "data" : [ "foo", "bar", "baz" ] }
object(stdClass)#%d (2) {
  ["__pclass"]=>
  object(MongoDB\BSON\Binary)#%d (2) {
    ["data"]=>
    string(11) "LiquidClass"
    ["type"]=>
    int(128)
  }
  ["data"]=>
  object(stdClass)#%d (3) {
    [0]=>
    string(3) "foo"
    [1]=>
    string(3) "bar"
    [2]=>
    string(3) "baz"
  }
}

{ "data" : { "__pclass" : { "$type": "80", "$binary" : "TGlxdWlkQ2xhc3M=" }, "breakfast" : "cheese", "dinner" : "sausages" } }
object(stdClass)#%d (1) {
  ["data"]=>
  object(stdClass)#%d (3) {
    ["__pclass"]=>
    object(MongoDB\BSON\Binary)#%d (2) {
      ["data"]=>
      string(11) "LiquidClass"
      ["type"]=>
      int(128)
    }
    ["breakfast"]=>
    string(6) "cheese"
    ["dinner"]=>
    string(8) "sausages"
  }
}

{ "__pclass": { "$type": "80", "$binary" : "TGlxdWlkQ2xhc3M=" }, "data" : { "__pclass": { "$type": "80", "$binary" : "TGlxdWlkQ2xhc3M=" }, "breakfast" : "cheese", "dinner" : "sausages" } }
object(stdClass)#%d (2) {
  ["__pclass"]=>
  object(MongoDB\BSON\Binary)#%d (2) {
    ["data"]=>
    string(11) "LiquidClass"
    ["type"]=>
    int(128)
  }
  ["data"]=>
  object(stdClass)#%d (3) {
    ["__pclass"]=>
    object(MongoDB\BSON\Binary)#%d (2) {
      ["data"]=>
      string(11) "LiquidClass"
      ["type"]=>
      int(128)
    }
    ["breakfast"]=>
    string(6) "cheese"
    ["dinner"]=>
    string(8) "sausages"
  }
}
