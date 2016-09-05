--TEST--
MongoDB\BSON\UTCDateTime::__set_state()
--FILE--
<?php
$tests = [
    '0',
    '-1416445411987',
    '1416445411987',
];

foreach ($tests as $milliseconds) {
    var_export(MongoDB\BSON\UTCDateTime::__set_state([
        'milliseconds' => $milliseconds,
    ]));
    echo "\n\n";
}

?>
--EXPECTF--
MongoDB\BSON\UTCDateTime::__set_state(array(
%w'milliseconds' => %r'?%r0%r'?%r,
))

MongoDB\BSON\UTCDateTime::__set_state(array(
%w'milliseconds' => %r'?%r-1416445411987%r'?%r,
))

MongoDB\BSON\UTCDateTime::__set_state(array(
%w'milliseconds' => %r'?%r1416445411987%r'?%r,
))

