--TEST--
BSON serialization: type wrapped Decimal128
--FILE--
<?php
$tests = [
	[ 'types' => [] ],
	[ 'types' => [ 'Decimal128' => 'MyDecimal128' ] ],
	[ 'root' => 'array', 'types' => [ 'Decimal128' => 'MyDecimal128' ] ],
	[ 'types' => [ 'Decimal128' => 'MyDecimal128ReturnScalarType' ] ],
];

class MyDecimal128 implements \MongoDB\BSON\TypeWrapper
{
	public $intern;

	public function __construct(\MongoDB\BSON\Type $type)
	{
		$this->intern = $type;
	}

	static function createFromBSONType(\MongoDB\BSON\Type $type) : \MongoDB\BSON\TypeWrapper
	{
		if (! $type instanceof \MongoDB\BSON\Decimal128) {
			throw Exception( "Not of the right type" );
		}

		return new self( $type );
	}

	function toBSONType()
	{
		return $this->intern;
	}
}

class MyDecimal128ReturnScalarType implements \MongoDB\BSON\TypeWrapper
{
	public $intern;

	static function createFromBSONType(\MongoDB\BSON\Type $type)
	{
		if (! $type instanceof \MongoDB\BSON\Decimal128) {
			throw Exception( "Not of the right type" );
		}

		return (float) $type->__toString();
	}

	function toBSONType()
	{
		return $this->intern;
	}
}

foreach ( $tests as $test )
{
	$bson = MongoDB\BSON\fromPHP( [ "decimal128" => new \MongoDB\BSON\Decimal128( "123456.1469702464" ) ] );
	$test = MongoDB\BSON\toPHP( $bson, $test );

	var_dump( $test );
}
?>
--EXPECTF--
object(stdClass)#%d (%d) {
  ["decimal128"]=>
  object(MongoDB\BSON\Decimal128)#%d (%d) {
    ["dec"]=>
    string(17) "123456.1469702464"
  }
}
object(stdClass)#%d (%d) {
  ["decimal128"]=>
  object(MyDecimal128)#%d (%d) {
    ["intern"]=>
    object(MongoDB\BSON\Decimal128)#%d (%d) {
      ["dec"]=>
      string(17) "123456.1469702464"
    }
  }
}
array(%d) {
  ["decimal128"]=>
  object(MyDecimal128)#%d (%d) {
    ["intern"]=>
    object(MongoDB\BSON\Decimal128)#%d (%d) {
      ["dec"]=>
      string(17) "123456.1469702464"
    }
  }
}
object(stdClass)#%d (%d) {
  ["decimal128"]=>
  float(123456.14697025)
}
