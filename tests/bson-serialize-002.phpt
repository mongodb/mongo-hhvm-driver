--TEST--
Test for MongoDB\BSON\Serializable #2
--SKIPIF--
<?php if (!defined("HHVM_VERSION_ID")) exit("skip HHVM encodes __pclass last"); ?>
--FILE--
<?php
require_once __DIR__ . "/utils.inc";
require_once __DIR__ . "/classes.inc";

$hannes = new Person("Hannes", 42);
$sunnyvale = new Address(94086, "USA");
$kopavogur = new Address(200, "Iceland");
$hannes->addAddress($sunnyvale);
$hannes->addAddress($kopavogur);

$mikola = new Person("Jeremy", 21);
$michigan = new Address(48169, "USA");

$hannes->addFriend($mikola);

var_dump($hannes);

$s = MongoDB\BSON\fromPHP(array($hannes));
echo "Test ", MongoDB\BSON\toJSON($s), "\n";
hex_dump($s);
$ret = MongoDB\BSON\toPHP($s);
var_dump($ret);
?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
object(Person)#%d (5) {
  ["name":protected]=>
  string(6) "Hannes"
  ["age":protected]=>
  int(42)
  ["addresses":protected]=>
  array(2) {
    [0]=>
    object(Address)#%d (2) {
      ["zip":protected]=>
      int(94086)
      ["country":protected]=>
      string(3) "USA"
    }
    [1]=>
    object(Address)#%d (2) {
      ["zip":protected]=>
      int(200)
      ["country":protected]=>
      string(7) "Iceland"
    }
  }
  ["friends":protected]=>
  array(1) {
    [0]=>
    object(Person)#%d (5) {
      ["name":protected]=>
      string(6) "Jeremy"
      ["age":protected]=>
      int(21)
      ["addresses":protected]=>
      array(0) {
      }
      ["friends":protected]=>
      array(0) {
      }
      ["secret":protected]=>
      string(24) "Jeremy confidential info"
    }
  }
  ["secret":protected]=>
  string(24) "Hannes confidential info"
}
Test { "0" : { "name" : "Hannes", "age" : 42, "addresses" : [ { "zip" : 94086, "country" : "USA", "__pclass" : { "$binary" : "QWRkcmVzcw==", "$type" : "80" } }, { "zip" : 200, "country" : "Iceland", "__pclass" : { "$binary" : "QWRkcmVzcw==", "$type" : "80" } } ], "friends" : [ { "name" : "Jeremy", "age" : 21, "addresses" : [  ], "friends" : [  ], "__pclass" : { "$binary" : "UGVyc29u", "$type" : "80" } } ], "__pclass" : { "$binary" : "UGVyc29u", "$type" : "80" } } }
     0 : 23 01 00 00 03 30 00 1b 01 00 00 02 6e 61 6d 65  [#....0......name]
    10 : 00 07 00 00 00 48 61 6e 6e 65 73 00 10 61 67 65  [.....Hannes..age]
    20 : 00 2a 00 00 00 04 61 64 64 72 65 73 73 65 73 00  [.*....addresses.]
    30 : 79 00 00 00 03 30 00 35 00 00 00 10 7a 69 70 00  [y....0.5....zip.]
    40 : 86 6f 01 00 02 63 6f 75 6e 74 72 79 00 04 00 00  [.o...country....]
    50 : 00 55 53 41 00 05 5f 5f 70 63 6c 61 73 73 00 07  [.USA..__pclass..]
    60 : 00 00 00 80 41 64 64 72 65 73 73 00 03 31 00 39  [....Address..1.9]
    70 : 00 00 00 10 7a 69 70 00 c8 00 00 00 02 63 6f 75  [....zip......cou]
    80 : 6e 74 72 79 00 08 00 00 00 49 63 65 6c 61 6e 64  [ntry.....Iceland]
    90 : 00 05 5f 5f 70 63 6c 61 73 73 00 07 00 00 00 80  [..__pclass......]
    A0 : 41 64 64 72 65 73 73 00 00 04 66 72 69 65 6e 64  [Address...friend]
    B0 : 73 00 5a 00 00 00 03 30 00 52 00 00 00 02 6e 61  [s.Z....0.R....na]
    C0 : 6d 65 00 07 00 00 00 4a 65 72 65 6d 79 00 10 61  [me.....Jeremy..a]
    D0 : 67 65 00 15 00 00 00 04 61 64 64 72 65 73 73 65  [ge......addresse]
    E0 : 73 00 05 00 00 00 00 04 66 72 69 65 6e 64 73 00  [s.......friends.]
    F0 : 05 00 00 00 00 05 5f 5f 70 63 6c 61 73 73 00 06  [......__pclass..]
   100 : 00 00 00 80 50 65 72 73 6f 6e 00 00 05 5f 5f 70  [....Person...__p]
   110 : 63 6c 61 73 73 00 06 00 00 00 80 50 65 72 73 6f  [class......Perso]
   120 : 6e 00 00                                         [n..]
object(stdClass)#%d (1) {
  [0]=>
  object(Person)#%d (5) {
    ["name":protected]=>
    string(6) "Hannes"
    ["age":protected]=>
    int(42)
    ["addresses":protected]=>
    array(2) {
      [0]=>
      object(Address)#%d (2) {
        ["zip":protected]=>
        int(94086)
        ["country":protected]=>
        string(3) "USA"
      }
      [1]=>
      object(Address)#%d (2) {
        ["zip":protected]=>
        int(200)
        ["country":protected]=>
        string(7) "Iceland"
      }
    }
    ["friends":protected]=>
    array(1) {
      [0]=>
      object(Person)#%d (5) {
        ["name":protected]=>
        string(6) "Jeremy"
        ["age":protected]=>
        int(21)
        ["addresses":protected]=>
        array(0) {
        }
        ["friends":protected]=>
        array(0) {
        }
        ["secret":protected]=>
        string(4) "none"
      }
    }
    ["secret":protected]=>
    string(4) "none"
  }
}
===DONE===
