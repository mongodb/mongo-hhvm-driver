--TEST--
BSON deserialization: unserialize root document as named class (pclass fallbacks)
--FILE--
<?php
class FallbackClass implements MongoDB\BSON\Unserializable
{
	function bsonUnserialize( array $map )
	{
		$this->fallback = $map;
	}
}

class UnserializableClass implements MongoDB\BSON\Unserializable
{
	function bsonUnserialize( array $map )
	{
		$this->unserialized = $map;
	}
}

class LiquidClass 
{
}

$tests = [
	'{ "foo" : 42 }',
	'{ "foo" : "yes", "__pclass": "RigidClass" }',
	'{ "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "RG9lc05vdEV4aXN0" } }', // DoesNotExist
	'{ "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "VW5zZXJpYWxpemFibGVDbGFzcw==" } }', // UnserializableClass
	'{ "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "TGlxdWlkQ2xhc3M=" } }', // LiquidClass

	'{ "nested" : { "foo" : 42 } }',
	'{ "nested" : { "foo" : "yes", "__pclass": "RigidClass" } }',
	'{ "nested" : { "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "RG9lc05vdEV4aXN0" } } }', // DoesNotExist
	'{ "nested" : { "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "VW5zZXJpYWxpemFibGVDbGFzcw==" } } }', // UnserializableClass
	'{ "nested" : { "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "TGlxdWlkQ2xhc3M=" } } }', // LiquidClass
];

$typemaps = [
	[ 'root' => 'FallbackClass' ],
];

foreach ( $typemaps as $typemap )
{
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
}
?>
--EXPECTF--
{ "foo" : 42 }
object(FallbackClass)#%d (1) {
  ["fallback"]=>
  array(1) {
    ["foo"]=>
    int(42)
  }
}

{ "foo" : "yes", "__pclass": "RigidClass" }
object(FallbackClass)#%d (1) {
  ["fallback"]=>
  array(2) {
    ["foo"]=>
    string(3) "yes"
    ["__pclass"]=>
    string(10) "RigidClass"
  }
}

{ "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "RG9lc05vdEV4aXN0" } }
object(FallbackClass)#%d (1) {
  ["fallback"]=>
  array(2) {
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

{ "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "VW5zZXJpYWxpemFibGVDbGFzcw==" } }
object(FallbackClass)#%d (1) {
  ["fallback"]=>
  array(2) {
    ["foo"]=>
    string(3) "yes"
    ["__pclass"]=>
    object(MongoDB\BSON\Binary)#%d (2) {
      ["data"]=>
      string(19) "UnserializableClass"
      ["type"]=>
      int(128)
    }
  }
}

{ "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "TGlxdWlkQ2xhc3M=" } }
object(FallbackClass)#%d (1) {
  ["fallback"]=>
  array(2) {
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

{ "nested" : { "foo" : 42 } }
object(FallbackClass)#%d (1) {
  ["fallback"]=>
  array(1) {
    ["nested"]=>
    object(stdClass)#%d (1) {
      ["foo"]=>
      int(42)
    }
  }
}

{ "nested" : { "foo" : "yes", "__pclass": "RigidClass" } }
object(FallbackClass)#%d (1) {
  ["fallback"]=>
  array(1) {
    ["nested"]=>
    object(stdClass)#%d (2) {
      ["foo"]=>
      string(3) "yes"
      ["__pclass"]=>
      string(10) "RigidClass"
    }
  }
}

{ "nested" : { "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "RG9lc05vdEV4aXN0" } } }
object(FallbackClass)#%d (1) {
  ["fallback"]=>
  array(1) {
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
}

{ "nested" : { "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "VW5zZXJpYWxpemFibGVDbGFzcw==" } } }
object(FallbackClass)#%d (1) {
  ["fallback"]=>
  array(1) {
    ["nested"]=>
    object(stdClass)#%d (2) {
      ["foo"]=>
      string(3) "yes"
      ["__pclass"]=>
      object(MongoDB\BSON\Binary)#%d (2) {
        ["data"]=>
        string(19) "UnserializableClass"
        ["type"]=>
        int(128)
      }
    }
  }
}

{ "nested" : { "foo" : "yes", "__pclass": { "$type": "80", "$binary" : "TGlxdWlkQ2xhc3M=" } } }
object(FallbackClass)#%d (1) {
  ["fallback"]=>
  array(1) {
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
}
