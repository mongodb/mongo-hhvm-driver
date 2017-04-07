--TEST--
MongoDB\BSON\toExtendedJSON(): Encoding JSON
--FILE--
<?php

$tests = [
    [],
    [ 'foo' => 'bar' ],
    [ 'foo' => [ 1, 2, 3 ]],
    [ 'foo' => [ 'bar' => 1 ]],
];

foreach ($tests as $value) {
    $bson = MongoDB\BSON\fromPHP($value);
    echo MongoDB\BSON\toExtendedJSON($bson), "\n";
}

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
{ }
{ "foo" : "bar" }
{ "foo" : [ { "$numberInt" : "1" }, { "$numberInt" : "2" }, { "$numberInt" : "3" } ] }
{ "foo" : { "bar" : { "$numberInt" : "1" } } }
===DONE===
