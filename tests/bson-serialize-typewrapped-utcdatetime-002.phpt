--TEST--
BSON serialization: type wrapped UTCDateTime to string
--FILE--
<?php
class MyUTCDateTime implements \MongoDB\BSON\TypeWrapper
{
	public $intern;

	public function __construct(\MongoDB\BSON\Type $type)
	{
		$this->intern = $type;
	}

	static function createFromBSONType(\MongoDB\BSON\Type $type) : \MongoDB\BSON\TypeWrapper
	{
		if (! $type instanceof \MongoDB\BSON\UTCDateTime) {
			throw Exception( "Not of the right type" );
		}

		return new self( $type );
	}

	function toBSONType()
	{
		return $this->intern->toDateTime()->format( \DateTime::RFC2822 );
	}
}

$var = new MyUTCDateTime( new \MongoDB\BSON\UTCDateTime( strtotime( "2016-08-02 13:07:07 UTC" ) * 1000 ) );
$bson = MongoDB\BSON\fromPHP( [ 'utcdatetime' => $var ] );
$result = MongoDB\BSON\toPHP( $bson );

var_dump( $result );
?>
--EXPECTF--
object(stdClass)#%d (%d) {
  ["utcdatetime"]=>
  string(31) "Tue, 02 Aug 2016 13:07:07 +0000"
}
