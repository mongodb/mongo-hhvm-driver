--TEST--
BSON serialization: type wrapped Regex
--FILE--
<?php
class MyRegex implements \MongoDB\BSON\TypeWrapper
{
	public $intern;

	public function __construct(\MongoDB\BSON\Type $type)
	{
		$this->intern = $type;
	}

	static function createFromBSONType(\MongoDB\BSON\Type $type) : \MongoDB\BSON\TypeWrapper
	{
		if (! $type instanceof \MongoDB\BSON\Regex) {
			throw Exception( "Not of the right type" );
		}

		return new self( $type );
	}

	function toBSONType()
	{
		return new \MongoDB\BSON\Regex( "^foo", 'm' );
	}
}

$var = new MyRegex( new \MongoDB\BSON\Regex( '^bar', 'i' ) );
$bson = MongoDB\BSON\fromPHP( [ 'regex' => $var ] );
$result = MongoDB\BSON\toPHP( $bson );

var_dump( $result );
?>
--EXPECTF--
object(stdClass)#%d (%d) {
  ["regex"]=>
  object(MongoDB\BSON\Regex)#%d (%d) {
    ["pattern"]=>
    string(4) "^foo"
    ["flags"]=>
    string(1) "m"
  }
}
