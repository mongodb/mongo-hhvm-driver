--TEST--
BSON deserialization - hex dump test (2)
--SKIPIF--
<?php if (!defined("HHVM_VERSION_ID")) exit("skip HHVM encodes __pclass last"); ?>
--FILE--
<?php
include dirname(__FILE__) . '/utils.inc';

class MyClass implements MongoDB\BSON\Persistable {
    function bsonSerialize() {
        return array(
            "random" => "class",
            "data"
        );
    }
    function bsonUnserialize(array $data) {
        $this->props = $data;
    }
}
class MyClass2 implements MongoDB\BSON\Persistable {
    function bsonSerialize() {
        return array(
            1, 2, 3,
        );
    }
    function bsonUnserialize(array $data) {
        $this->props = $data;
    }
}

$tests = array(
    array("stuff" => new MyClass),
    array("stuff" => new MyClass2),
    array("stuff" => array(new MyClass, new MyClass2)),
);

foreach($tests as $n => $test) {
    $s = MongoDB\BSON\fromPHP($test);
    echo "Test#{$n} ", MongoDB\BSON\toJSON($s), "\n";
    hex_dump($s);
    $ret = MongoDB\BSON\toPHP($s);
    var_dump($ret);
}
?>
--EXPECTF--
Test#0 { "stuff" : { "random" : "class", "0" : "data", "__pclass" : { "$type" : "80", "$binary" : "TXlDbGFzcw==" } } }
     0 : 45 00 00 00 03 73 74 75 66 66 00 39 00 00 00 02  [E....stuff.9....]
    10 : 72 61 6e 64 6f 6d 00 06 00 00 00 63 6c 61 73 73  [random.....class]
    20 : 00 02 30 00 05 00 00 00 64 61 74 61 00 05 5f 5f  [..0.....data..__]
    30 : 70 63 6c 61 73 73 00 07 00 00 00 80 4d 79 43 6c  [pclass......MyCl]
    40 : 61 73 73 00 00                                   [ass..]
object(stdClass)#%d (1) {
  ["stuff"]=>
  object(MyClass)#%d (1) {
    ["props"]=>
    array(3) {
      ["random"]=>
      string(5) "class"
      [0]=>
      string(4) "data"
      ["__pclass"]=>
      object(%SBSON\Binary)#%d (2) {
        ["data"]=>
        string(7) "MyClass"
        ["type"]=>
        int(128)
      }
    }
  }
}
Test#1 { "stuff" : { "0" : 1, "1" : 2, "2" : 3, "__pclass" : { "$type" : "80", "$binary" : "TXlDbGFzczI=" } } }
     0 : 3d 00 00 00 03 73 74 75 66 66 00 31 00 00 00 10  [=....stuff.1....]
    10 : 30 00 01 00 00 00 10 31 00 02 00 00 00 10 32 00  [0......1......2.]
    20 : 03 00 00 00 05 5f 5f 70 63 6c 61 73 73 00 08 00  [.....__pclass...]
    30 : 00 00 80 4d 79 43 6c 61 73 73 32 00 00           [...MyClass2..]
object(stdClass)#%d (1) {
  ["stuff"]=>
  object(MyClass2)#%d (1) {
    ["props"]=>
    array(4) {
      [0]=>
      int(1)
      [1]=>
      int(2)
      [2]=>
      int(3)
      ["__pclass"]=>
      object(%SBSON\Binary)#%d (2) {
        ["data"]=>
        string(8) "MyClass2"
        ["type"]=>
        int(128)
      }
    }
  }
}
Test#2 { "stuff" : [ { "random" : "class", "0" : "data", "__pclass" : { "$type" : "80", "$binary" : "TXlDbGFzcw==" } }, { "0" : 1, "1" : 2, "2" : 3, "__pclass" : { "$type" : "80", "$binary" : "TXlDbGFzczI=" } } ] }
     0 : 81 00 00 00 04 73 74 75 66 66 00 75 00 00 00 03  [.....stuff.u....]
    10 : 30 00 39 00 00 00 02 72 61 6e 64 6f 6d 00 06 00  [0.9....random...]
    20 : 00 00 63 6c 61 73 73 00 02 30 00 05 00 00 00 64  [..class..0.....d]
    30 : 61 74 61 00 05 5f 5f 70 63 6c 61 73 73 00 07 00  [ata..__pclass...]
    40 : 00 00 80 4d 79 43 6c 61 73 73 00 03 31 00 31 00  [...MyClass..1.1.]
    50 : 00 00 10 30 00 01 00 00 00 10 31 00 02 00 00 00  [...0......1.....]
    60 : 10 32 00 03 00 00 00 05 5f 5f 70 63 6c 61 73 73  [.2......__pclass]
    70 : 00 08 00 00 00 80 4d 79 43 6c 61 73 73 32 00 00  [......MyClass2..]
    80 : 00                                               [.]
object(stdClass)#%d (1) {
  ["stuff"]=>
  array(2) {
    [0]=>
    object(MyClass)#%d (1) {
      ["props"]=>
      array(3) {
        ["random"]=>
        string(5) "class"
        [0]=>
        string(4) "data"
        ["__pclass"]=>
        object(%SBSON\Binary)#%d (2) {
          ["data"]=>
          string(7) "MyClass"
          ["type"]=>
          int(128)
        }
      }
    }
    [1]=>
    object(MyClass2)#%d (1) {
      ["props"]=>
      array(4) {
        [0]=>
        int(1)
        [1]=>
        int(2)
        [2]=>
        int(3)
        ["__pclass"]=>
        object(%SBSON\Binary)#%d (2) {
          ["data"]=>
          string(8) "MyClass2"
          ["type"]=>
          int(128)
        }
      }
    }
  }
}
