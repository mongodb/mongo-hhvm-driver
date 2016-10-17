--TEST--
BSON serialization: type wrapped Timestamp
--FILE--
<?php
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
			throw Exception( "Not of the right type" );
		}

		return new self( $type );
	}

	function toBSONType()
	{
		return new \MongoDB\BSON\Timestamp( 5678, 1234 );
	}
}

$var = new MyTimestamp( new \MongoDB\BSON\Timestamp( 1234, 5678 ) );
$bson = MongoDB\BSON\fromPHP( [ 'timestamp' => $var ] );
$result = MongoDB\BSON\toPHP( $bson );

var_dump( $result );
?>
--EXPECTF--
object(stdClass)#%d (%d) {
  ["timestamp"]=>
  object(MongoDB\BSON\Timestamp)#%d (%d) {
    ["increment"]=>
    string(4) "5678"
    ["timestamp"]=>
    string(4) "1234"
  }
}
