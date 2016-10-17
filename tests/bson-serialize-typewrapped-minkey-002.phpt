--TEST--
BSON serialization: type wrapped MinKey to MIN_INT
--FILE--
<?php
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
		return PHP_INT_MIN;
	}
}

$var = new MyMinKey( new \MongoDB\BSON\MinKey() );
$bson = MongoDB\BSON\fromPHP( [ 'minkey' => $var ] );
$result = MongoDB\BSON\toPHP( $bson );

var_dump( $result );
?>
--EXPECTF--
object(stdClass)#%d (%d) {
  ["minkey"]=>
  int(-%d)
}
