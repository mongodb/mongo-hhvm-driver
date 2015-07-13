--TEST--
BSON serialization: PHP serializable object with packed array return
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
		return array_values( $this->primes );
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
	$bson = MongoDB\BSON\fromPHP( $test );
	echo MongoDB\BSON\toJSON( $bson ), "\n";
}
?>
--EXPECT--
{ "primes" : [ 2, 3, 5, 7, 8, 13, 17, 19 ] }
