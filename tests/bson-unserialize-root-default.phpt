--TEST--
BSON deserialization: BSON root defaults
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
	'{ "foo" : 42 }',
	'{ "foo" : "yes", "__pclass": "RigidClass" }',
	'{ "foo" : "yes", "__pclass": { "$type": "44", "$binary" : "UmlnaWRDbGFzcw==" } }',
	'{ "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "RG9lc05vdEV4aXN0" } }', // DoesNotExist
	'{ "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "Tm9TZXJpYWxpemU=" } }', // NoSerialize
	'{ "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "TGlxdWlkQ2xhc3M=" } }', // LiquidClass
	'{ "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "UmlnaWRDbGFzcw==" } }', // RigidClass
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
{ "foo" : 42 }
object(stdClass)#1 (1) {
  ["foo"]=>
  int(42)
}

{ "foo" : "yes", "__pclass": "RigidClass" }
object(stdClass)#1 (2) {
  ["foo"]=>
  string(3) "yes"
  ["__pclass"]=>
  string(10) "RigidClass"
}

{ "foo" : "yes", "__pclass": { "$type": "44", "$binary" : "UmlnaWRDbGFzcw==" } }
object(stdClass)#2 (2) {
  ["foo"]=>
  string(3) "yes"
  ["__pclass"]=>
  object(MongoDB\BSON\Binary)#1 (2) {
    ["data":"MongoDB\BSON\Binary":private]=>
    string(10) "RigidClass"
    ["subType":"MongoDB\BSON\Binary":private]=>
    int(68)
  }
}

{ "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "RG9lc05vdEV4aXN0" } }
object(stdClass)#3 (2) {
  ["foo"]=>
  string(3) "yes"
  ["__pclass"]=>
  object(MongoDB\BSON\Binary)#2 (2) {
    ["data":"MongoDB\BSON\Binary":private]=>
    string(12) "DoesNotExist"
    ["subType":"MongoDB\BSON\Binary":private]=>
    int(128)
  }
}

{ "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "Tm9TZXJpYWxpemU=" } }
object(stdClass)#5 (2) {
  ["foo"]=>
  string(3) "yes"
  ["__pclass"]=>
  object(MongoDB\BSON\Binary)#3 (2) {
    ["data":"MongoDB\BSON\Binary":private]=>
    string(11) "NoSerialize"
    ["subType":"MongoDB\BSON\Binary":private]=>
    int(128)
  }
}

{ "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "TGlxdWlkQ2xhc3M=" } }
object(stdClass)#7 (2) {
  ["foo"]=>
  string(3) "yes"
  ["__pclass"]=>
  object(MongoDB\BSON\Binary)#5 (2) {
    ["data":"MongoDB\BSON\Binary":private]=>
    string(11) "LiquidClass"
    ["subType":"MongoDB\BSON\Binary":private]=>
    int(128)
  }
}

{ "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "UmlnaWRDbGFzcw==" } }
object(RigidClass)#8 (2) {
  ["foo"]=>
  string(3) "yes"
  ["pclass"]=>
  string(10) "RigidClass"
}
