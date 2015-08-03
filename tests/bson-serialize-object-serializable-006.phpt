--TEST--
BSON serialization: PHP serializable object: examples from spec
--FILE--
<?php
class MyClass
{
	public $foo = 42;
	protected $prot = "wine";
	private $fpr = "cheese";
}

class AnotherClass1 implements MongoDB\BSON\Serializable
{
	public $foo = 42;
	protected $prot = "wine";
	private $fpr = "cheese";
	function bsonSerialize() {
		return [ 'foo' => $this->foo, 'prot' => $this->prot ];
	}
}

class AnotherClass2 implements MongoDB\BSON\Serializable
{
	public $foo = 42;
	function bsonSerialize() {
		return $this;
	}
}

class AnotherClass3 implements MongoDB\BSON\Serializable
{
	private $elements = [ 'foo', 'bar' ];
	function bsonSerialize() {
		return $this->elements;
	}
}

class AnotherClass4 implements MongoDB\BSON\Serializable
{
	private $elements = [ 0 => 'foo', 2 => 'bar' ];
	function bsonSerialize()
	{
		return $this->elements;
	}
}

class AnotherClass5 implements MongoDB\BSON\Serializable
{
	private $elements = [ 0 => 'foo', 2 => 'bar' ];
	function bsonSerialize()
	{
		return array_values( $this->elements );
	}
}

class AnotherClass6 implements MongoDB\BSON\Serializable
{
	private $elements = [ 'foo', 'bar' ];
	function bsonSerialize()
	{
		return (object) $this->elements;
	}
}

class ContainerClass implements MongoDB\BSON\Serializable
{
	public $things;

	function __construct( $things )
	{
		$this->things = $things;
	}

	function bsonSerialize()
	{
		return [ 'things' => $this->things ];
	}
}

class UpperClass implements MongoDB\BSON\Persistable
{
	public $foo = 42;
	protected $prot = "wine";
	private $fpr = "cheese";
	function bsonSerialize() {
		return [ 'foo' => $this->foo, 'prot' => $this->prot ];
	}
	function bsonUnserialize( array $a ) {}
}

$a = new stdClass;
$a->foo = 42;

$tests = [
	$a,
	new MyClass,
	new AnotherClass1,
	new AnotherClass3,
	new ContainerClass( new AnotherClass4 ),
	new ContainerClass( new AnotherClass5 ),
	new ContainerClass( new AnotherClass6 ),
	new UpperClass,

	// throw exceptions
	new AnotherClass2,
];

foreach ( $tests as $test )
{
	try
	{
		$bson = MongoDB\BSON\fromPHP( $test );
		echo MongoDB\BSON\toJSON( $bson ), "\n";
	}
	catch ( MongoDB\Driver\Exception\UnexpectedValueException $e )
	{
		echo $e->getMessage(), "\n";
	}

}
?>
--EXPECT--
{ "foo" : 42 }
{ "foo" : 42 }
{ "foo" : 42, "prot" : "wine" }
{ "0" : "foo", "1" : "bar" }
{ "things" : { "0" : "foo", "2" : "bar" } }
{ "things" : [ "foo", "bar" ] }
{ "things" : { "0" : "foo", "1" : "bar" } }
{ "foo" : 42, "prot" : "wine", "__pclass" : { "$type" : "80", "$binary" : "VXBwZXJDbGFzcw==" } }
Expected AnotherClass2::bsonSerialize() to return an array or stdClass, AnotherClass2 given
