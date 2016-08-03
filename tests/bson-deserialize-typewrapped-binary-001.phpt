--TEST--
BSON serialization: type wrapped Binary
--FILE--
<?php
$tests = [
	[ 'types' => [] ],
	[ 'types' => [ 'Binary' => 'MyBinary' ] ],
	[ 'root' => 'array', 'types' => [ 'Binary' => 'MyBinary' ] ],
	[ 'types' => [ 'Binary' => 'MyBinaryReturnScalarType' ] ],
];

class MyBinary implements \MongoDB\BSON\TypeWrapper
{
	public $intern;

	public function __construct(\MongoDB\BSON\Type $type)
	{
		$this->intern = $type;
	}

	static function createFromBSONType(\MongoDB\BSON\Type $type) : \MongoDB\BSON\TypeWrapper
	{
		if (! $type instanceof \MongoDB\BSON\Binary) {
			throw Exception( "Not of the right type" );
		}

		return new self( $type );
	}

	function toBSONType()
	{
		return $this->intern;
	}
}

class MyBinaryReturnScalarType implements \MongoDB\BSON\TypeWrapper
{
	public $intern;

	public function __construct(\MongoDB\BSON\Type $type)
	{
		$this->intern = $type;
	}

	static function createFromBSONType(\MongoDB\BSON\Type $type)
	{
		if (! $type instanceof \MongoDB\BSON\Binary) {
			throw Exception( "Not of the right type" );
		}

		return 42;
	}

	function toBSONType()
	{
		return $this->intern;
	}
}

foreach ( $tests as $test )
{
	$bson = MongoDB\BSON\fromPHP( [ 'binary' => new \MongoDB\BSON\Binary( "1234", 1 ) ] );
	$test = MongoDB\BSON\toPHP( $bson, $test );

	var_dump( $test );
}
?>
--EXPECTF--
object(stdClass)#%d (%d) {
  ["binary"]=>
  object(MongoDB\BSON\Binary)#%d (%d) {
    ["data"]=>
    string(4) "1234"
    ["type"]=>
    int(1)
  }
}
object(stdClass)#%d (%d) {
  ["binary"]=>
  object(MyBinary)#%d (%d) {
    ["intern"]=>
    object(MongoDB\BSON\Binary)#%d (%d) {
      ["data"]=>
      string(4) "1234"
      ["type"]=>
      int(1)
    }
  }
}
array(1) {
  ["binary"]=>
  object(MyBinary)#%d (%d) {
    ["intern"]=>
    object(MongoDB\BSON\Binary)#%d (%d) {
      ["data"]=>
      string(4) "1234"
      ["type"]=>
      int(1)
    }
  }
}
object(stdClass)#%d (%d) {
  ["binary"]=>
  int(42)
}
