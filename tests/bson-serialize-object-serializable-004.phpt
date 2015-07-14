--TEST--
BSON serialization: PHP serializable object with stdClass return
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
		return (object) $this->primes;
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
{ "primes" : { "0" : 2, "1" : 3, "2" : 5, "3" : 7, "4" : 8, "6" : 13, "7" : 17, "8" : 19 } }
