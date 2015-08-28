--TEST--
BSON deserialization - hex dump test (1)
--SKIPIF--
<?php if (!defined("HHVM_VERSION_ID")) exit("skip HHVM encodes __pclass last"); ?>
--FILE--
<?php
include dirname(__FILE__) . '/utils.inc';

class MyDocument implements MongoDB\BSON\Persistable
{
    public $data;

    public function __construct()
    {
        $this->data = array(
            'list' => array(1, 2, 3),
            'map' => (object) array('foo' => 'bar'),
        );
    }

    public function bsonSerialize()
    {
        return $this->data;
    }

    public function bsonUnserialize(array $data)
    {
        foreach (array('list', 'map') as $key) {
            if (isset($data[$key])) {
                $this->data[$key] = $data[$key];
            }
        }
    }
}

$bson = MongoDB\BSON\fromPHP(new MyDocument);
echo "Test ", MongoDB\BSON\toJSON($bson), "\n";
hex_dump($bson);

$typeMap = array(
    'array' => null,
    'document' => null,
    'root' => null,
);

var_dump(MongoDB\BSON\toPHP($bson, $typeMap));

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test { "list" : [ 1, 2, 3 ], "map" : { "foo" : "bar" }, "__pclass" : { "$type" : "80", "$binary" : "TXlEb2N1bWVudA==" } }
     0 : 55 00 00 00 04 6c 69 73 74 00 1a 00 00 00 10 30  [U....list......0]
    10 : 00 01 00 00 00 10 31 00 02 00 00 00 10 32 00 03  [......1......2..]
    20 : 00 00 00 00 03 6d 61 70 00 12 00 00 00 02 66 6f  [.....map......fo]
    30 : 6f 00 04 00 00 00 62 61 72 00 00 05 5f 5f 70 63  [o.....bar...__pc]
    40 : 6c 61 73 73 00 0a 00 00 00 80 4d 79 44 6f 63 75  [lass......MyDocu]
    50 : 6d 65 6e 74 00                                   [ment.]
object(MyDocument)#%d (1) {
  ["data"]=>
  array(2) {
    ["list"]=>
    array(3) {
      [0]=>
      int(1)
      [1]=>
      int(2)
      [2]=>
      int(3)
    }
    ["map"]=>
    object(stdClass)#%d (1) {
      ["foo"]=>
      string(3) "bar"
    }
  }
}
===DONE===
