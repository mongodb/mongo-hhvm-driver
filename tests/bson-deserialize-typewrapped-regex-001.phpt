--TEST--
BSON serialization: type wrapped Regex
--FILE--
<?php
$tests = [
	[ 'types' => [] ],
	[ 'types' => [ 'Regex' => 'MyRegex' ] ],
	[ 'root' => 'array', 'types' => [ 'Regex' => 'MyRegex' ] ],
	[ 'types' => [ 'Regex' => 'MyRegexReturnScalarType' ] ],
];

class MyRegex implements \MongoDB\BSON\TypeWrapper
{
	public $intern;

	public function __construct(\MongoDB\BSON\Type $type)
	{
		$this->intern = $type;
	}

	static function createFromBSONType(\MongoDB\BSON\Type $type) : \MongoDB\BSON\TypeWrapper
	{
		if (! $type instanceof \MongoDB\BSON\Regex) {
			echo "Not of the right type\n";
		}

		return new self( $type );
	}

	function toBSONType()
	{
		return $this->intern;
	}
}

class MyRegexReturnScalarType implements \MongoDB\BSON\TypeWrapper
{
	public $intern;

	static function createFromBSONType(\MongoDB\BSON\Type $type)
	{
		if (! $type instanceof \MongoDB\BSON\Regex) {
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
	$bson = MongoDB\BSON\fromPHP( [ "regex" => new \MongoDB\BSON\Regex( '^foo', 'i' ) ] );
	$test = MongoDB\BSON\toPHP( $bson, $test );

	var_dump( $test );
}
?>
--EXPECTF--
object(stdClass)#%d (%d) {
  ["regex"]=>
  object(MongoDB\BSON\Regex)#%d (%d) {
    ["pattern"]=>
    string(4) "^foo"
    ["flags"]=>
    string(1) "i"
  }
}
object(stdClass)#%d (%d) {
  ["regex"]=>
  object(MyRegex)#%d (%d) {
    ["intern"]=>
    object(MongoDB\BSON\Regex)#%d (%d) {
      ["pattern"]=>
      string(4) "^foo"
      ["flags"]=>
      string(1) "i"
    }
  }
}
array(%d) {
  ["regex"]=>
  object(MyRegex)#%d (%d) {
    ["intern"]=>
    object(MongoDB\BSON\Regex)#%d (%d) {
      ["pattern"]=>
      string(4) "^foo"
      ["flags"]=>
      string(1) "i"
    }
  }
}
object(stdClass)#%d (%d) {
  ["regex"]=>
  string(7) "/^foo/i"
}
