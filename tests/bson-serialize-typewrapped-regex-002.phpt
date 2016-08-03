--TEST--
BSON serialization: type wrapped Regex to string
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
		return $this->intern->__toString();
	}
}

$var = new MyRegex( new \MongoDB\BSON\Regex( "^bar", 'is' ) );
$bson = MongoDB\BSON\fromPHP( [ 'regex' => $var ] );
$result = MongoDB\BSON\toPHP( $bson );

var_dump( $result );
?>
--EXPECTF--
object(stdClass)#%d (%d) {
  ["regex"]=>
  string(8) "/^bar/is"
}
