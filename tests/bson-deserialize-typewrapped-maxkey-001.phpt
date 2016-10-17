--TEST--
BSON serialization: type wrapped MaxKey
--FILE--
<?php
$tests = [
	[ 'types' => [] ],
	[ 'types' => [ 'MaxKey' => 'MyMaxKey' ] ],
	[ 'root' => 'array', 'types' => [ 'MaxKey' => 'MyMaxKey' ] ],
	[ 'types' => [ 'MaxKey' => 'MyMaxKeyReturnScalarType' ] ],
];

class MyMaxKey implements \MongoDB\BSON\TypeWrapper
{
	public $intern;

	public function __construct(\MongoDB\BSON\Type $type)
	{
		$this->intern = $type;
	}

	static function createFromBSONType(\MongoDB\BSON\Type $type) : \MongoDB\BSON\TypeWrapper
	{
		if (! $type instanceof \MongoDB\BSON\MaxKey) {
			throw Exception( "Not of the right type" );
		}

		return new self( $type );
	}

	function toBSONType()
	{
		return $this->intern;
	}
}

class MyMaxKeyReturnScalarType implements \MongoDB\BSON\TypeWrapper
{
	public $intern;

	public function __construct(\MongoDB\BSON\Type $type)
	{
		$this->intern = $type;
	}

	static function createFromBSONType(\MongoDB\BSON\Type $type)
	{
		if (! $type instanceof \MongoDB\BSON\MaxKey) {
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
	$bson = MongoDB\BSON\fromPHP( [ 'maxkey' => new \MongoDB\BSON\MaxKey() ] );
	$test = MongoDB\BSON\toPHP( $bson, $test );

	var_dump( $test );
}
?>
--EXPECTF--
object(stdClass)#%d (%d) {
  ["maxkey"]=>
  object(MongoDB\BSON\MaxKey)#%d (%d) {
  }
}
object(stdClass)#%d (%d) {
  ["maxkey"]=>
  object(MyMaxKey)#%d (%d) {
    ["intern"]=>
    object(MongoDB\BSON\MaxKey)#%d (%d) {
    }
  }
}
array(%d) {
  ["maxkey"]=>
  object(MyMaxKey)#%d (%d) {
    ["intern"]=>
    object(MongoDB\BSON\MaxKey)#%d (%d) {
    }
  }
}
object(stdClass)#%d (%d) {
  ["maxkey"]=>
  int(42)
}
