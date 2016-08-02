--TEST--
BSON serialization: type wrapped ObjectID
--FILE--
<?php
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
			throw Exception( "Not of the right type" );
		}

		return new self( $type );
	}

	function toBSONType()
	{
		return new \MongoDB\BSON\ObjectID( "56732d3dda14d81214634922" );
	}
}

$var = new MyObjectID( new \MongoDB\BSON\ObjectID( "56732d3dda14d81214600000" ) );
$bson = MongoDB\BSON\fromPHP( [ 'objectid' => $var ] );
$result = MongoDB\BSON\toPHP( $bson );

var_dump( $result );
?>
--EXPECTF--
object(stdClass)#%d (%d) {
  ["objectid"]=>
  object(MongoDB\BSON\ObjectID)#%d (%d) {
    ["oid"]=>
    string(24) "56732d3dda14d81214634922"
  }
}
