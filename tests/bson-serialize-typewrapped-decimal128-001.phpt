--TEST--
BSON serialization: type wrapped Decimal128
--FILE--
<?php
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
		return new \MongoDB\BSON\Decimal128( "5678e1234" );
	}
}

$var = new MyDecimal128( new \MongoDB\BSON\Decimal128( "1234e5678" ) );
$bson = MongoDB\BSON\fromPHP( [ 'decimal128' => $var ] );
$result = MongoDB\BSON\toPHP( $bson );

var_dump( $result );
?>
--EXPECTF--
object(stdClass)#%d (%d) {
  ["decimal128"]=>
  object(MongoDB\BSON\Decimal128)#%d (%d) {
    ["dec"]=>
    string(11) "5.678E+1237"
  }
}
