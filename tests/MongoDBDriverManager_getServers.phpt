--TEST--
MongoDB\Driver\Manager::getServers()
--FILE--
<?php
function assertServerType($type) {
    if ($type === MongoDB\Driver\Server::TYPE_STANDALONE) {
        printf("Found standalone server type: %d\n", $type);
    } else {
        printf("Unexpected server type: %d\n", $type);
    }
}

$manager = new MongoDB\Driver\Manager();

$servers = $manager->getServers();
printf("Known servers: %d\n", count($servers));

echo "Pinging\n";
$command = new MongoDB\Driver\Command(array('ping' => 1));
$manager->executeCommand("test", $command);

$servers = $manager->getServers();
printf("Known servers: %d\n", count($servers));

foreach ($servers as $server) {
    printf("Found server: %s:%d\n", $server->getHost(), $server->getPort());
    assertServerType($server->getType());
}

?>
--EXPECTF--
Known servers: 0
Pinging
Known servers: 1
Found server: %s:%d
Found standalone server type: 1
