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
--EXPECTF--
{ "nested" : { "foo" : 42 } }
object(stdClass)#%d (1) {
  ["nested"]=>
  object(stdClass)#%d (1) {
    ["foo"]=>
    int(42)
  }
}

{ "nested" : { "foo" : "yes", "__pclass": "RigidClass" } }
object(stdClass)#%d (1) {
  ["nested"]=>
  object(stdClass)#%d (2) {
    ["foo"]=>
    string(3) "yes"
    ["__pclass"]=>
    string(10) "RigidClass"
  }
}

{ "nested" : { "foo" : "yes", "__pclass": { "$type": "44", "$binary" : "UmlnaWRDbGFzcw==" } } }
object(stdClass)#%d (1) {
  ["nested"]=>
  object(stdClass)#%d (2) {
    ["foo"]=>
    string(3) "yes"
    ["__pclass"]=>
    object(MongoDB\BSON\Binary)#%d (2) {
      ["data"]=>
      string(10) "RigidClass"
      ["type"]=>
      int(68)
    }
  }
}

{ "nested" : { "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "RG9lc05vdEV4aXN0" } } }
object(stdClass)#%d (1) {
  ["nested"]=>
  object(stdClass)#%d (2) {
    ["foo"]=>
    string(3) "yes"
    ["__pclass"]=>
    object(MongoDB\BSON\Binary)#%d (2) {
      ["data"]=>
      string(12) "DoesNotExist"
      ["type"]=>
      int(128)
    }
  }
}

{ "nested" : { "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "Tm9TZXJpYWxpemU=" } } }
object(stdClass)#%d (1) {
  ["nested"]=>
  object(stdClass)#%d (2) {
    ["foo"]=>
    string(3) "yes"
    ["__pclass"]=>
    object(MongoDB\BSON\Binary)#%d (2) {
      ["data"]=>
      string(11) "NoSerialize"
      ["type"]=>
      int(128)
    }
  }
}

{ "nested" : { "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "TGlxdWlkQ2xhc3M=" } } }
object(stdClass)#%d (1) {
  ["nested"]=>
  object(stdClass)#%d (2) {
    ["foo"]=>
    string(3) "yes"
    ["__pclass"]=>
    object(MongoDB\BSON\Binary)#%d (2) {
      ["data"]=>
      string(11) "LiquidClass"
      ["type"]=>
      int(128)
    }
  }
}

{ "nested" : { "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "UmlnaWRDbGFzcw==" } } }
object(stdClass)#%d (1) {
  ["nested"]=>
  object(RigidClass)#%d (2) {
    ["foo"]=>
    string(3) "yes"
    ["pclass"]=>
    string(10) "RigidClass"
  }
}
