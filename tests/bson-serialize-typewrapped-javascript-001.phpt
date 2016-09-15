--TEST--
BSON serialization: type wrapped Javascript
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
		return new \MongoDB\BSON\Javascript( 'function() { return 2; }' );
	}
}

$var = new MyJavascript( new \MongoDB\BSON\Javascript( 'function() { return 1; }' ) );
$bson = MongoDB\BSON\fromPHP( [ 'javascript' => $var ] );
$result = MongoDB\BSON\toPHP( $bson );

var_dump( $result );
?>
--EXPECTF--
object(stdClass)#%d (%d) {
  ["javascript"]=>
  object(MongoDB\BSON\Javascript)#%d (%d) {
    ["code"]=>
    string(24) "function() { return 2; }"
    ["scope"]=>
    NULL
  }
}
