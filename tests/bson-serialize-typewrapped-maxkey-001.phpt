--TEST--
BSON serialization: type wrapped MaxKey
--FILE--
<?php
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
		return new \MongoDB\BSON\MaxKey();
	}
}

$var = new MyMaxKey( new \MongoDB\BSON\MaxKey() );
$bson = MongoDB\BSON\fromPHP( [ 'maxkey' => $var ] );
$result = MongoDB\BSON\toPHP( $bson );

var_dump( $result );
?>
--EXPECTF--
object(stdClass)#%d (%d) {
  ["maxkey"]=>
  object(MongoDB\BSON\MaxKey)#%d (%d) {
  }
}
