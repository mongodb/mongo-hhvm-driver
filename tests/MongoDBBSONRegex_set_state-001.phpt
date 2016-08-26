--TEST--
MongoDB\BSON\Regex::__set_state()
--FILE--
<?php
var_export(MongoDB\BSON\Regex::__set_state([
    'pattern' => 'regexp',
    'flags' => 'i',
]));
echo "\n";

?>
--EXPECT--
MongoDB\BSON\Regex::__set_state(array(
  'pattern' => 'regexp',
  'flags' => 'i',
))
