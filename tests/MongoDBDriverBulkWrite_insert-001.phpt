--TEST--
MongoDB\Driver\BulkWrite::insert
--FILE--
<?php
$bw = new MongoDB\Driver\BulkWrite;

$_id = $bw->insert( [ 'test' => 'foo' ] );
echo get_class( $_id ), "\n";
echo strlen( $_id ), "\n";
echo $bw->count(), "\n";
?>
--EXPECT--
MongoDB\BSON\ObjectID
24
1
