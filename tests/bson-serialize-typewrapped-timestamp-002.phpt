--TEST--
BSON serialization: type wrapped Timestamp to string
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
		$s = $this->intern->__toString();
		preg_match( '/^\[\d+:(\d+)\]$/', $s, $matches );
		return date_create( "@{$matches[1]}" )->format( \DateTime::RFC2822 );
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
  string(31) "Thu, 01 Jan 1970 01:34:38 +0000"
}
