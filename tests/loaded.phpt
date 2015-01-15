--TEST--
Test for loaded extension
--FILE--
<?php
$extensions = get_loaded_extensions();
if (in_array('mongodb', $extensions)) {
	echo "The MongoDB HHVM extension is loaded\n";
}
?>
--EXPECT--
The MongoDB HHVM extension is loaded
