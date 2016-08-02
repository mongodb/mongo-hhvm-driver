--TEST--
BSON serialization: type wrapped Binary
--FILE--
<?php
class MyBinary implements \MongoDB\BSON\TypeWrapper
{
	public $intern;

	public function __construct(\MongoDB\BSON\Type $type)
	{
		$this->intern = $type;
	}

	static function createFromBSONType(\MongoDB\BSON\Type $type) : \MongoDB\BSON\TypeWrapper
	{
		if (! $type instanceof \MongoDB\BSON\Binary) {
			throw Exception( "Not of the right type" );
		}

		return new self( $type );
	}

	function toBSONType()
	{
		return new \MongoDB\BSON\Binary( "5678", 2 );
	}
}

$var = new MyBinary( new \MongoDB\BSON\Binary( "1234", 1 ) );
$bson = MongoDB\BSON\fromPHP( [ 'binary' => $var ] );
$result = MongoDB\BSON\toPHP( $bson );

var_dump( $result );
?>
--EXPECTF--
object(stdClass)#%d (%d) {
  ["binary"]=>
  object(MongoDB\BSON\Binary)#%d (%d) {
    ["data"]=>
    string(4) "5678"
    ["type"]=>
    int(2)
  }
}
