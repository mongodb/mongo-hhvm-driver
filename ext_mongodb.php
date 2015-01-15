<?hh
namespace MongoDB;

<<__NativeData("MongoDBManager")>>
class Manager {
	<<__Native>>
	function __construct(string $dsn = "localhost", array $options = array(), array $driverOptions = array());

	function executeCommand(string $db, MongoDB\Driver\Command $command, MongoDB\Driver\ReadPreference $readPreference = null): MongoDB\Driver\CommandResult
	{
	}

	function executeQuery(string $namespace, MongoDB\Driver\Query $query, MongoDB\Driver\ReadPreference $readPreference = null): MongoDB\Driver\QueryResult
	{
	}

	function executeWriteBatch(string $namespace, MongoDB\Driver\WriteBatch $batch, MongoDB\Driver\WriteConcern $writeConcern = null): MongoDB\Driver\WriteResult
	{
	}

	<<__Native>>
	function executeInsert(string $namespace, mixed $document, ?MongoDB\Driver\WriteConcern $writeConcern = null): MongoDB\Driver\WriteResult;

	function executeUpdate(string $namespace, $query, $newObj, array $updateOptions = array(), MongoDB\Driver\WriteConcern $writeConcern = null): MongoDB\Driver\WriteResult
	{
	}

	function executeDelete(string $namespace, $query, array $deleteOptions = array(), MongoDB\Driver\WriteConcern $writeConcern = null): MongoDB\Driver\WriteResult
	{
	}

	function getServers(): void
	{
	}
}

namespace MongoDB\Driver;

class Exception {
}

class Utils {
	const ERROR_INVALID_ARGUMENT  = 1;
	const ERROR_RUNTIME           = 2;
	const ERROR_MONGOC_FAILED     = 3;
	const ERROR_WRITE_FAILED      = 4;
	const ERROR_CONNECTION_FAILED = 5;

	static function throwHippoException($domain, $message)
	{
		switch ($domain) {
			case self::ERROR_INVALID_ARGUMENT:
				throw new \InvalidArgumentException($message);

			case self::ERROR_RUNTIME:
			case self::ERROR_MONGOC_FAILED:
				throw new RuntimeException($mssage);

			case self::ERROR_WRITE_FAILED:
				throw new WriteException($message);

			case self::ERROR_CONNECTION_FAILED:
				throw new ConnectionException($message);
		}
	}

	static function mustBeArrayOrObject(string $name, mixed $value)
	{
		$valueType = gettype($value);
		if (!in_array($valueType, [ 'array', 'object' ])) {
			Utils::throwHippoException(
				Utils::ERROR_INVALID_ARGUMENT,
				"Expected {$name} to be array or object, {$valueType} given"
			);
		}
	}
}

class RuntimeException extends Exception {
}

final class WriteResult {
	private $nUpserted = 0;
	private $nMatched = 0;
	private $nRemoved = 0;
	private $nInserted = 0;
	protected $nModified = 0;
	private $upsertedIds = null;
	private $writeErrors = null;
	private $writeConcernError = null;
	private $info = null;

	public function getInsertedCount() { return $this->nInserted; }
	public function getMatchedCount()  { return $this->nMatched; }
	public function getModifiedCount() { return $this->nModified; }
	public function getDeletedCount()  { return $this->nDeleted; }
	public function getUpsertedCount() { return $this->nUpserted; }

	public function getInfo()
	{
		if ($this->info && gettype($this->info) == 'array') {
			return $this->info;
		}
		return [];
	}

	public function getServer()
	{
		throw new \Exception("getServer is not implemented yet");
	}

	public function getUpsertedIds(): array
	{
		if ($this->upsertedIds && gettype($this->upsertedIds) == 'array') {
			return $this->upsertedIds;
		}
		return [];
	}

	public function getWriteConcernError(): MongoDB\WriteConcernError
	{
		if ($this->writeConcernError && gettype($this->writeConcernError) == 'object') {
			return $this->writeConcernError;
		}
		return false;
	}

	public function getWriteErrors(): array
	{
		if ($this->writeErrors && gettype($this->writeErrors) == 'array') {
			return $this->writeErrors;
		}
		return [];
	}
}

namespace MongoDB\BSON;

interface Type
{
}

class Regex implements Type
{
	function __construct(private string $pattern, private string $flags)
	{
	}

	function getPattern() : string
	{
		return $this->pattern;
	}

	function getFlags() : string
	{
		return $this->flags;
	}

	function __toString() : string
	{
		return "/{$this->pattern}/{$this->flags}";
	}
}
