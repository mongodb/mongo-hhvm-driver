--TEST--
BSON serialization: type wrapped UTCDateTime
--FILE--
<?php
$tests = [
	[ 'types' => [] ],
	[ 'types' => [ 'UTCDateTime' => 'MyUTCDateTime' ] ],
	[ 'root' => 'array', 'types' => [ 'UTCDateTime' => 'MyUTCDateTime' ] ],
	[ 'types' => [ 'UTCDateTime' => 'MyUTCDateTimeReturnDateTimeType' ] ],
];

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
			echo "Not of the right type\n";
		}

		return new self( $type );
	}

	function toBSONType() : \MongoDB\BSON\Type
	{
		return $this->intern;
	}
}

class MyUTCDateTimeReturnDateTimeType implements \MongoDB\BSON\TypeWrapper
{
	public $intern;

	static function createFromBSONType(\MongoDB\BSON\Type $type)
	{
		if (! $type instanceof \MongoDB\BSON\UTCDateTime) {
			echo "Not of the right type\n";
		}

		return $type->toDateTime();
	}

	function toBSONType()
	{
		return $this->intern;
	}
}

foreach ( $tests as $test )
{
	$bson = MongoDB\BSON\fromPHP( [ "utcdatetime" => new \MongoDB\BSON\UTCDateTime( 1469702464000 ) ] );
	$test = MongoDB\BSON\toPHP( $bson, $test );

	var_dump( $test );
}
?>
--EXPECTF--
object(stdClass)#%d (%d) {
  ["utcdatetime"]=>
  object(MongoDB\BSON\UTCDateTime)#%d (%d) {
    ["milliseconds"]=>
    string(13) "1469702464000"
  }
}
object(stdClass)#%d (%d) {
  ["utcdatetime"]=>
  object(MyUTCDateTime)#%d (%d) {
    ["intern"]=>
    object(MongoDB\BSON\UTCDateTime)#%d (%d) {
      ["milliseconds"]=>
      string(13) "1469702464000"
    }
  }
}
array(%d) {
  ["utcdatetime"]=>
  object(MyUTCDateTime)#%d (%d) {
    ["intern"]=>
    object(MongoDB\BSON\UTCDateTime)#%d (%d) {
      ["milliseconds"]=>
      string(13) "1469702464000"
    }
  }
}
object(stdClass)#%d (%d) {
  ["utcdatetime"]=>
  object(DateTime)#%d (%d) {
    ["date"]=>
    string(26) "2016-07-28 10:41:04.000000"
    ["timezone_type"]=>
    int(3)
    ["timezone"]=>
    string(3) "UTC"
  }
}
