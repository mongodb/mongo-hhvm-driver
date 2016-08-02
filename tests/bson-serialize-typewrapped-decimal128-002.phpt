--TEST--
BSON serialization: type wrapped Decimal128 cast to float
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
		return (float) $this->intern->__toString();
	}
}

$var = new MyDecimal128( new \MongoDB\BSON\Decimal128( "2345e67" ) );
$bson = MongoDB\BSON\fromPHP( [ 'decimal128' => $var ] );
$result = MongoDB\BSON\toPHP( $bson );

var_dump( $result );
?>
--EXPECTF--
object(stdClass)#%d (%d) {
  ["decimal128"]=>
  float(2.345E+70)
}
