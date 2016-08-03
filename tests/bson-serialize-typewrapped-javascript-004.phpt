--TEST--
BSON serialization: type wrapped Javascript (w\scope) to string
--FILE--
<?php
class MyJavascript implements \MongoDB\BSON\TypeWrapper
{
	public $intern;

	public function __construct(\MongoDB\BSON\Type $type)
	{
		$this->intern = $type;
	}

	static function createFromBSONType(\MongoDB\BSON\Type $type) : \MongoDB\BSON\TypeWrapper
	{
		if (! $type instanceof \MongoDB\BSON\Javascript) {
			throw Exception( "Not of the right type" );
		}

		return new self( $type );
	}

	function toBSONType()
	{
		return $this->intern->getCode() . '; ' . print_r( (array) $this->intern->getScope(), true );
	}
}

$var = new MyJavascript( new \MongoDB\BSON\Javascript( 'function() { return 1; }', [ 'foo' => 'one' ] ) );
$bson = MongoDB\BSON\fromPHP( [ 'javascript' => $var ] );
$result = MongoDB\BSON\toPHP( $bson );

var_dump( $result );
?>
--EXPECTF--
object(stdClass)#%d (%d) {
  ["javascript"]=>
  string(53) "function() { return 1; }; Array
(
    [foo] => one
)
"
}
