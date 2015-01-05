<?php
var_dump(extension_loaded("mongodb"));

$m = new MongoDB\Manager("mongodb://localhost:27017");
?>
