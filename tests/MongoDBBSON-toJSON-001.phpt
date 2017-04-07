--TEST--
MongoDB\BSON\toJSON(): Encoding JSON
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
    echo MongoDB\BSON\toJSON($bson), "\n";
}

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
{ }
{ "foo" : "bar" }
{ "foo" : [ 1, 2, 3 ] }
{ "foo" : { "bar" : 1 } }
===DONE===
