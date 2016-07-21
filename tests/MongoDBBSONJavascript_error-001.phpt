--TEST--
MongoDB\BSON\Javascript::__construct() does not allow code to contain null bytes
--FILE--
<?php
try
{
	new \MongoDB\BSON\Javascript("function foo() { return 'Something with \0 in it'; }");
}
catch ( \MongoDB\Driver\Exception\InvalidArgumentException $e )
{
	echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Code cannot contain null bytes
