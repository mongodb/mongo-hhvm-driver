<?hh
namespace MongoDB;

<<__NativeData("MongoDBManager")>>
class Manager {
	<<__Native>>
	function __construct(string $dsn = "localhost", array $options = array(), array $driverOptions = array()): void;

	<<__Native>>
	function __destruct(): void;
}
