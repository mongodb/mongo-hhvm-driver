--TEST--
BSON serialization: type wrapped MinKey
--FILE--
<?php
$tests = [
	[ 'types' => [] ],
	[ 'types' => [ 'MinKey' => 'MyMinKey' ] ],
	[ 'root' => 'array', 'types' => [ 'MinKey' => 'MyMinKey' ] ],
	[ 'types' => [ 'MinKey' => 'MyMinKeyReturnScalarType' ] ],
];

class MyMinKey implements \MongoDB\BSON\TypeWrapper
{
	public $intern;

	public function __construct(\MongoDB\BSON\Type $type)
	{
		$this->intern = $type;
	}

	static function createFromBSONType(\MongoDB\BSON\Type $type) : \MongoDB\BSON\TypeWrapper
	{
		if (! $type instanceof \MongoDB\BSON\MinKey) {
			throw Exception( "Not of the right type" );
		}

		return new self( $type );
	}

	function toBSONType()
	{
		return $this->intern;
	}
}

class MyMinKeyReturnScalarType implements \MongoDB\BSON\TypeWrapper
{
	public $intern;

	public function __construct(\MongoDB\BSON\Type $type)
	{
		$this->intern = $type;
	}

	static function createFromBSONType(\MongoDB\BSON\Type $type)
	{
		if (! $type instanceof \MongoDB\BSON\MinKey) {
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
	$bson = MongoDB\BSON\fromPHP( [ 'minkey' => new \MongoDB\BSON\MinKey() ] );
	$test = MongoDB\BSON\toPHP( $bson, $test );

	var_dump( $test );
}
?>
--EXPECTF--
object(stdClass)#%d (%d) {
  ["minkey"]=>
  object(MongoDB\BSON\MinKey)#%d (%d) {
  }
}
object(stdClass)#%d (%d) {
  ["minkey"]=>
  object(MyMinKey)#%d (%d) {
    ["intern"]=>
    object(MongoDB\BSON\MinKey)#%d (%d) {
    }
  }
}
array(%d) {
  ["minkey"]=>
  object(MyMinKey)#%d (%d) {
    ["intern"]=>
    object(MongoDB\BSON\MinKey)#%d (%d) {
    }
  }
}
object(stdClass)#%d (%d) {
  ["minkey"]=>
  int(42)
}
