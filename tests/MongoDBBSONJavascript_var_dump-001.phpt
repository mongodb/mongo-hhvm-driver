--TEST--
MongoDB\BSON\Javascript (var_dump())
--FILE--
<?php
$tests = [
    ['function foo(bar) { return bar; }', null],
    ['function foo(bar) { return bar; }', []],
    ['function foo() { return foo; }', ['foo' => 42]],
    ['function foo() { return id; }', ['id' => new MongoDB\BSON\ObjectId('53e2a1c40640fd72175d4603')]],
];

foreach ($tests as $test) {
    list($code, $scope) = $test;

    var_dump(MongoDB\BSON\Javascript::__set_state([
        'code' => $code,
        'scope' => $scope,
    ]));
    echo "\n\n";
}

// Test with missing scope field
var_dump(MongoDB\BSON\Javascript::__set_state([
    'code' => 'function foo(bar) { return bar; }',
]));
echo "\n\n";

?>
--EXPECTF--
object(MongoDB\BSON\Javascript)#%d (2) {
  ["code"]=>
  string(33) "function foo(bar) { return bar; }"
  ["scope"]=>
  NULL
}


object(MongoDB\BSON\Javascript)#%d (2) {
  ["code"]=>
  string(33) "function foo(bar) { return bar; }"
  ["scope"]=>
  object(stdClass)#%d (0) {
  }
}


object(MongoDB\BSON\Javascript)#%d (2) {
  ["code"]=>
  string(30) "function foo() { return foo; }"
  ["scope"]=>
  object(stdClass)#%d (1) {
    ["foo"]=>
    int(42)
  }
}


object(MongoDB\BSON\Javascript)#%d (2) {
  ["code"]=>
  string(29) "function foo() { return id; }"
  ["scope"]=>
  object(stdClass)#%d (1) {
    ["id"]=>
    object(MongoDB\BSON\ObjectID)#%d (1) {
      ["oid"]=>
      string(24) "53e2a1c40640fd72175d4603"
    }
  }
}


object(MongoDB\BSON\Javascript)#%d (2) {
  ["code"]=>
  string(33) "function foo(bar) { return bar; }"
  ["scope"]=>
  NULL
}
