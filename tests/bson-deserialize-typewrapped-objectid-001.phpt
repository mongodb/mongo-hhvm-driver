--TEST--
BSON serialization: type wrapped ObjectID
--FILE--
<?php
$tests = [
	[ 'types' => [] ],
	[ 'types' => [ 'ObjectID' => 'MyObjectID' ] ],
	[ 'root' => 'array', 'types' => [ 'ObjectID' => 'MyObjectID' ] ],
	[ 'types' => [ 'ObjectID' => 'MyObjectIDReturnScalarType' ] ],
];

class MyObjectID implements \MongoDB\BSON\TypeWrapper
{
	public $intern;

	public function __construct(\MongoDB\BSON\Type $type)
	{
		$this->intern = $type;
	}

	static function createFromBSONType(\MongoDB\BSON\Type $type) : \MongoDB\BSON\TypeWrapper
	{
		if (! $type instanceof \MongoDB\BSON\ObjectID) {
			echo "Not of the right type\n";
		}

		return new self( $type );
	}

	function toBSONType()
	{
		return $this->intern;
	}
}

class MyObjectIDReturnScalarType implements \MongoDB\BSON\TypeWrapper
{
	public $intern;

	public function __construct(\MongoDB\BSON\Type $type)
	{
		$this->intern = $type;
	}

	static function createFromBSONType(\MongoDB\BSON\Type $type)
	{
		if (! $type instanceof \MongoDB\BSON\ObjectID) {
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
	$bson = MongoDB\BSON\fromPHP( [ 'objectid' => new \MongoDB\BSON\ObjectID() ] );
	$test = MongoDB\BSON\toPHP( $bson, $test );

	var_dump( $test );
}
?>
--EXPECTF--
object(stdClass)#%d (%d) {
  ["objectid"]=>
  object(MongoDB\BSON\ObjectID)#%d (%d) {
    ["oid"]=>
    string(24) "%s"
  }
}
object(stdClass)#%d (%d) {
  ["objectid"]=>
  object(MyObjectID)#%d (%d) {
    ["intern"]=>
    object(MongoDB\BSON\ObjectID)#%d (%d) {
      ["oid"]=>
      string(24) "%s"
    }
  }
}
array(%d) {
  ["objectid"]=>
  object(MyObjectID)#%d (%d) {
    ["intern"]=>
    object(MongoDB\BSON\ObjectID)#%d (%d) {
      ["oid"]=>
      string(24) "%s"
    }
  }
}
object(stdClass)#%d (%d) {
  ["objectid"]=>
  string(24) "%s"
}
