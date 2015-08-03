--TEST--
BSON serialization: PHP serializable object with invalid return
--FILE--
<?php
class PrimeNumbers implements MongoDB\BSON\Serializable
{
	public $primes;

	function __construct( $primes )
	{
		$this->primes = $primes;

		unset( $this->primes[5] );
	}

	function bsonSerialize()
	{
		return $this;
	}
}

class Wrapper
{
	public $primes;

	function __construct()
	{
		$this->primes = new PrimeNumbers( [ 2, 3, 5, 7, 8, 11, 13, 17, 19 ] );
	}
}

$tests = [
	new Wrapper
];

foreach ( $tests as $test )
{
	try
	{
		$bson = MongoDB\BSON\fromPHP( $test );
	}
	catch ( MongoDB\Driver\Exception\UnexpectedValueException $e )
	{
		echo $e->getMessage(), "\n";
	}
}
?>
--EXPECTF--
Expected PrimeNumbers::bsonSerialize() to return an array or stdClass, PrimeNumbers given
