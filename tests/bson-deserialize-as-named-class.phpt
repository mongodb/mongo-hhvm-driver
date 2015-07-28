--TEST--
BSON deserialization: unserialize as named class
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
		if ( array_key_exists( '__pclass', $map ) )
		{
			if ( is_object( $map['__pclass'] ) && $map['__pclass'] instanceof MongoDB\BSON\Binary )
			{
				$this->pclass = $map['__pclass']->getData();
			}
		}
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

$typemap = [ 'root' => 'LiquidClass' ];

foreach ( $tests as $test )
{
	echo $test, "\n";
	$bson = MongoDB\BSON\fromJSON( $test );
	try
	{
		var_dump( MongoDB\BSON\toPHP( $bson, $typemap ) );
	}
	catch ( Exception $e )
	{
		echo $e->getMessage(), "\n";
	}
	echo "\n";
}
?>
--EXPECT--
{ "foo" : 42 }
object(LiquidClass)#1 (1) {
  ["foo"]=>
  int(42)
}

{ "foo" : "yes", "__pclass": "RigidClass" }
object(LiquidClass)#1 (1) {
  ["foo"]=>
  string(3) "yes"
}

{ "foo" : "yes", "__pclass": { "$type": "44", "$binary" : "UmlnaWRDbGFzcw==" } }
object(LiquidClass)#2 (2) {
  ["foo"]=>
  string(3) "yes"
  ["pclass"]=>
  string(10) "RigidClass"
}

{ "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "RG9lc05vdEV4aXN0" } }
object(LiquidClass)#3 (2) {
  ["foo"]=>
  string(3) "yes"
  ["pclass"]=>
  string(12) "DoesNotExist"
}

{ "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "Tm9TZXJpYWxpemU=" } }
object(LiquidClass)#4 (2) {
  ["foo"]=>
  string(3) "yes"
  ["pclass"]=>
  string(11) "NoSerialize"
}

{ "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "TGlxdWlkQ2xhc3M=" } }
object(LiquidClass)#5 (2) {
  ["foo"]=>
  string(3) "yes"
  ["pclass"]=>
  string(11) "LiquidClass"
}

{ "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "UmlnaWRDbGFzcw==" } }
object(RigidClass)#6 (2) {
  ["foo"]=>
  string(3) "yes"
  ["pclass"]=>
  string(10) "RigidClass"
}
