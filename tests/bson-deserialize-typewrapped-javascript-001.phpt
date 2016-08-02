--TEST--
BSON serialization: type wrapped Javascript
--FILE--
<?php
$tests = [
	[ 'types' => [] ],
	[ 'types' => [ 'Javascript' => 'MyJavascript' ] ],
	[ 'root' => 'array', 'types' => [ 'Javascript' => 'MyJavascript' ] ],
	[ 'types' => [ 'Javascript' => 'MyJavascriptReturnScalarType' ] ],
];

class MyJavascript implements \MongoDB\BSON\TypeWrapper
{
	public $intern;

	public function __construct(\MongoDB\BSON\Type $type)
	{
		$this->intern = $type;
	}

	static function createFromBSONType(\MongoDB\BSON\Type $type) : \MongoDB\BSON\TypeWrapper
	{
		if (! $type instanceof \MongoDB\BSON\Javascript) {
			echo "Not of the right type\n";
		}

		return new self( $type );
	}

	function toBSONType()
	{
		return $this->intern;
	}
}

class MyJavascriptReturnScalarType implements \MongoDB\BSON\TypeWrapper
{
	public $intern;

	static function createFromBSONType(\MongoDB\BSON\Type $type)
	{
		if (! $type instanceof \MongoDB\BSON\Javascript) {
			echo "Not of the right type\n";
		}

		return $type->__toString();
	}

	function toBSONType()
	{
		return $this->intern;
	}
}

foreach ( $tests as $test )
{
	$bson = MongoDB\BSON\fromPHP( [ "javascript" => new \MongoDB\BSON\Javascript( 'function() { return foo; }' ) ] );
	$result = MongoDB\BSON\toPHP( $bson, $test );
	var_dump( $result );

	$bson = MongoDB\BSON\fromPHP( [ "javascript" => new \MongoDB\BSON\Javascript( 'function() { return foo; }', ['foo' => 'bar'] ) ] );
	$result = MongoDB\BSON\toPHP( $bson, $test );
	var_dump( $result );
}
?>
--EXPECTF--
object(stdClass)#%d (%d) {
  ["javascript"]=>
  object(MongoDB\BSON\Javascript)#%d (%d) {
    ["code"]=>
    string(26) "function() { return foo; }"
    ["scope"]=>
    NULL
  }
}
object(stdClass)#%d (%d) {
  ["javascript"]=>
  object(MongoDB\BSON\Javascript)#%d (%d) {
    ["code"]=>
    string(26) "function() { return foo; }"
    ["scope"]=>
    object(stdClass)#%d (%d) {
      ["foo"]=>
      string(3) "bar"
    }
  }
}
object(stdClass)#%d (%d) {
  ["javascript"]=>
  object(MyJavascript)#%d (%d) {
    ["intern"]=>
    object(MongoDB\BSON\Javascript)#%d (%d) {
      ["code"]=>
      string(26) "function() { return foo; }"
      ["scope"]=>
      NULL
    }
  }
}
object(stdClass)#%d (%d) {
  ["javascript"]=>
  object(MyJavascript)#%d (%d) {
    ["intern"]=>
    object(MongoDB\BSON\Javascript)#%d (%d) {
      ["code"]=>
      string(26) "function() { return foo; }"
      ["scope"]=>
      object(stdClass)#%d (%d) {
        ["foo"]=>
        string(3) "bar"
      }
    }
  }
}
array(%d) {
  ["javascript"]=>
  object(MyJavascript)#%d (%d) {
    ["intern"]=>
    object(MongoDB\BSON\Javascript)#%d (%d) {
      ["code"]=>
      string(26) "function() { return foo; }"
      ["scope"]=>
      NULL
    }
  }
}
array(%d) {
  ["javascript"]=>
  object(MyJavascript)#%d (%d) {
    ["intern"]=>
    object(MongoDB\BSON\Javascript)#%d (%d) {
      ["code"]=>
      string(26) "function() { return foo; }"
      ["scope"]=>
      array(%d) {
        ["foo"]=>
        string(3) "bar"
      }
    }
  }
}
object(stdClass)#%d (%d) {
  ["javascript"]=>
  string(26) "function() { return foo; }"
}
object(stdClass)#%d (%d) {
  ["javascript"]=>
  string(26) "function() { return foo; }"
}
