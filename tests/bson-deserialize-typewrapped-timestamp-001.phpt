--TEST--
BSON serialization: type wrapped Timestamp
--FILE--
<?php
$tests = [
	[ 'types' => [] ],
	[ 'types' => [ 'Timestamp' => 'MyTimestamp' ] ],
	[ 'root' => 'array', 'types' => [ 'Timestamp' => 'MyTimestamp' ] ],
	[ 'types' => [ 'Timestamp' => 'MyTimestampReturnScalarType' ] ],
];

class MyTimestamp implements \MongoDB\BSON\TypeWrapper
{
	public $intern;

	public function __construct(\MongoDB\BSON\Type $type)
	{
		$this->intern = $type;
	}

	static function createFromBSONType(\MongoDB\BSON\Type $type) : \MongoDB\BSON\TypeWrapper
	{
		if (! $type instanceof \MongoDB\BSON\Timestamp) {
			echo "Not of the right type\n";
		}

		return new self( $type );
	}

	function toBSONType()
	{
		return $this->intern;
	}
}

class MyTimestampReturnScalarType implements \MongoDB\BSON\TypeWrapper
{
	public $intern;

	static function createFromBSONType(\MongoDB\BSON\Type $type)
	{
		if (! $type instanceof \MongoDB\BSON\Timestamp) {
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
	$bson = MongoDB\BSON\fromPHP( [ "timestamp" => new \MongoDB\BSON\Timestamp( 123456, 1469702464 ) ] );
	$test = MongoDB\BSON\toPHP( $bson, $test );

	var_dump( $test );
}
?>
--EXPECTF--
object(stdClass)#%d (%d) {
  ["timestamp"]=>
  object(MongoDB\BSON\Timestamp)#%d (%d) {
    ["increment"]=>
    string(6) "123456"
    ["timestamp"]=>
    string(10) "1469702464"
  }
}
object(stdClass)#%d (%d) {
  ["timestamp"]=>
  object(MyTimestamp)#%d (%d) {
    ["intern"]=>
    object(MongoDB\BSON\Timestamp)#%d (%d) {
      ["increment"]=>
      string(6) "123456"
      ["timestamp"]=>
      string(10) "1469702464"
    }
  }
}
array(%d) {
  ["timestamp"]=>
  object(MyTimestamp)#%d (%d) {
    ["intern"]=>
    object(MongoDB\BSON\Timestamp)#%d (%d) {
      ["increment"]=>
      string(6) "123456"
      ["timestamp"]=>
      string(10) "1469702464"
    }
  }
}
object(stdClass)#%d (%d) {
  ["timestamp"]=>
  string(19) "[123456:1469702464]"
}
