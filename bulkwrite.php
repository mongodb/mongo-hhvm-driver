<?php
$bw = new MongoDB\Driver\BulkWrite(true);

echo $bw->insert( [ 'test' => 'foo' ] ), "\n";
?>
