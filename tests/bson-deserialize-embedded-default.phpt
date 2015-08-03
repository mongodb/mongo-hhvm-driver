--TEST--
BSON deserialization: BSON embedded defaults
--FILE--
<?php
class NoSerialize
{
}

class LiquidClass implements MongoDB\BSON\Unserializable
{
	function bsonUnserialize( array $map )
	{
		$this->foo = $map['foo'];
		$this->pclass = $map['__pclass']->getData();
	}
}

class RigidClass extends LiquidClass implements MongoDB\BSON\Persistable
{
	function bsonSerialize()
	{
	}
}

$tests = [
	'{ "nested" : { "foo" : 42 } }',
	'{ "nested" : { "foo" : "yes", "__pclass": "RigidClass" } }',
	'{ "nested" : { "foo" : "yes", "__pclass": { "$type": "44", "$binary" : "UmlnaWRDbGFzcw==" } } }',
	'{ "nested" : { "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "RG9lc05vdEV4aXN0" } } }', // DoesNotExist
	'{ "nested" : { "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "Tm9TZXJpYWxpemU=" } } }', // NoSerialize
	'{ "nested" : { "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "TGlxdWlkQ2xhc3M=" } } }', // LiquidClass
	'{ "nested" : { "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "UmlnaWRDbGFzcw==" } } }', // RigidClass
];

foreach ( $tests as $test )
{
	echo $test, "\n";
	$bson = MongoDB\BSON\fromJSON( $test );
	var_dump( MongoDB\BSON\toPHP( $bson ) );
	echo "\n";
}
?>
--EXPECT--
{ "nested" : { "foo" : 42 } }
object(stdClass)#2 (1) {
  ["nested"]=>
  object(stdClass)#1 (1) {
    ["foo"]=>
    int(42)
  }
}

{ "nested" : { "foo" : "yes", "__pclass": "RigidClass" } }
object(stdClass)#3 (1) {
  ["nested"]=>
  object(stdClass)#2 (2) {
    ["foo"]=>
    string(3) "yes"
    ["__pclass"]=>
    string(10) "RigidClass"
  }
}

{ "nested" : { "foo" : "yes", "__pclass": { "$type": "44", "$binary" : "UmlnaWRDbGFzcw==" } } }
object(stdClass)#5 (1) {
  ["nested"]=>
  object(stdClass)#4 (2) {
    ["foo"]=>
    string(3) "yes"
    ["__pclass"]=>
    object(MongoDB\BSON\Binary)#3 (2) {
      ["data"]=>
      string(10) "RigidClass"
      ["type"]=>
      int(68)
    }
  }
}

{ "nested" : { "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "RG9lc05vdEV4aXN0" } } }
object(stdClass)#7 (1) {
  ["nested"]=>
  object(stdClass)#6 (2) {
    ["foo"]=>
    string(3) "yes"
    ["__pclass"]=>
    object(MongoDB\BSON\Binary)#5 (2) {
      ["data"]=>
      string(12) "DoesNotExist"
      ["type"]=>
      int(128)
    }
  }
}

{ "nested" : { "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "Tm9TZXJpYWxpemU=" } } }
object(stdClass)#10 (1) {
  ["nested"]=>
  object(stdClass)#9 (2) {
    ["foo"]=>
    string(3) "yes"
    ["__pclass"]=>
    object(MongoDB\BSON\Binary)#7 (2) {
      ["data"]=>
      string(11) "NoSerialize"
      ["type"]=>
      int(128)
    }
  }
}

{ "nested" : { "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "TGlxdWlkQ2xhc3M=" } } }
object(stdClass)#13 (1) {
  ["nested"]=>
  object(stdClass)#12 (2) {
    ["foo"]=>
    string(3) "yes"
    ["__pclass"]=>
    object(MongoDB\BSON\Binary)#10 (2) {
      ["data"]=>
      string(11) "LiquidClass"
      ["type"]=>
      int(128)
    }
  }
}

{ "nested" : { "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "UmlnaWRDbGFzcw==" } } }
object(stdClass)#15 (1) {
  ["nested"]=>
  object(RigidClass)#14 (2) {
    ["foo"]=>
    string(3) "yes"
    ["pclass"]=>
    string(10) "RigidClass"
  }
}
