--TEST--
BSON serialization: PHP serializable object with packed array return for root
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

$tests = [
	new PrimeNumbers( [ 2, 3, 5, 7, 8, 11, 13, 17 ] ),
];

foreach ( $tests as $test )
{
	$bson = MongoDB\BSON\fromPHP( $test );
	echo MongoDB\BSON\toJSON( $bson ), "\n";
}
?>
--EXPECT--
{ "0" : 2, "1" : 3, "2" : 5, "3" : 7, "4" : 8, "5" : 13, "6" : 17 }
