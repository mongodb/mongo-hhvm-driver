--TEST--
BSON serialization: type wrapped UTCDateTime
--FILE--
<?php
class MyUTCDateTime implements \MongoDB\BSON\TypeWrapper
{
	public $intern;

	public function __construct(\MongoDB\BSON\Type $type)
	{
		$this->intern = $type->toDateTime();
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
		$this->intern = $this->intern->modify( "+1 day" );

		return new \MongoDB\BSON\UTCDateTime( $this->intern->getTimestamp() * 1000 );
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
  object(MongoDB\BSON\UTCDateTime)#%d (%d) {
    ["milliseconds"]=>
    int(1470229627000)
  }
}
