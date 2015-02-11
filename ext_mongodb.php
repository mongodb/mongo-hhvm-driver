<?hh
namespace MongoDB\Driver;

<<__NativeData("MongoDBDriverManager")>>
class Manager {
	<<__Native>>
	function __construct(string $dsn = "localhost", array $options = array(), array $driverOptions = array());

	function executeCommand(string $db, MongoDB\Driver\Command $command, MongoDB\Driver\ReadPreference $readPreference = null): MongoDB\Driver\CommandResult
	{
	}

	<<__Native>>
	function executeQuery(string $namespace, MongoDB\Driver\Query $query, MongoDB\Driver\ReadPreference $readPreference = null): MongoDB\Driver\QueryResult;

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

/* {{{ Exception Classes */
class Exception {}
class RuntimeException extends Exception {}
/* }}} */

/* {{{ Cursor Classes */
<<__NativeData("MongoDBDriverCursorId")>>
final class CursorId {
	<<__Native>>
	public function __construct(string $id);

	<<__Native>>
	public function __toString() : string;
}

<<__NativeData("MongoDBDriverCursor")>>
final class Cursor {
	<<__Native>>
	public function getId() : MongoDB\Driver\CursorId;
}
/* }}} */

/* {{{ Value Classes */
final class Query {
	private array $query;

	public function __construct(mixed $filter, array $options = array())
	{
		$zquery = [];

		/* phongo_query_init */
		Utils::mustBeArrayOrObject('filter', $filter);

		if ($options) {
			$this->query['batchSize'] = array_key_exists('batchSize', $options ) ? (int) $options['batchSize'] : 0;
			$this->query['flags'] = array_key_exists('flags', $options ) ? (int) $options['flags'] : 0;
			$this->query['limit'] = array_key_exists('limit', $options ) ? (int) $options['limit'] : 0;
			$this->query['skip'] = array_key_exists('skip', $options ) ? (int) $options['skip'] : 0;
		}

		if (array_key_exists('modifiers', $options)) {
			Utils::mustBeArrayOrObject('modifiers', $options['modifiers']);
			$zquery += (array) $options['modifiers'];
		}

		if (array_key_exists('projection', $options)) {
			Utils::mustBeArrayOrObject('projection', $options['projection']);
			$this->query['fields'] = (array) $options['projection'];
		}

		if (array_key_exists('sort', $options)) {
			Utils::mustBeArrayOrObject('sort', $options['sort']);
			$this->query['query']['$orderby'] = (array) $options['sort'];
		}

		$this->query['query']['$query'] = $filter;
	}
}

<<__NativeData("MongoDBDriverQueryResult")>>
final class QueryResult {
	function __construct(private string $namespace, private Query $zquery, private ReadPreference $readPreference = null)
	{
	}

	public function setTypemap(array $typemap) : void
	{
		throw new \Exception("getTypemap is not implemented yet");
	}

	<<__Native>>
	public function getIterator() : Cursor;

	public function setIteratorClass() : QueryResult
	{
		throw new \Exception("setIteratorClass is not implemented yet");
	}

	public function setIteratorInitCallback() : QueryResult
	{
		throw new \Exception("setIteratorInitCallback is not implemented yet");
	}

	<<__Native>>
	public function getServer(): Server;
}

<<__NativeData("MongoDBDriverReadPreference")>>
final class ReadPreference {
	<<__Native>>
	private function _setReadPreference(int $readPreference): void;

	<<__Native>>
	private function _setReadPreferenceTags(array $tagSets): void;

	public function __construct(int $readPreference, mixed $tagSets = null)
	{
		if ($tagSets !== NULL && Utils::mustBeArrayOrObject('parameter 2', $tagSets)) {
			return;
		}

		switch ($readPreference) {
			case ReadPreference::RP_PRIMARY:
			case ReadPreference::RP_PRIMARY_PREFERRED:
			case ReadPreference::RP_SECONDARY:
			case ReadPreference::RP_SECONDARY_PREFERRED:
			case ReadPreference::RP_NEAREST:
				// calling into Native
				$this->_setReadPreference($readPreference);

				if ($tagSets) {
					// calling into Native, might throw exception
					$this->_setReadPreferenceTags($tagSets);
				}

				break;

			default:
				Utils::throwHippoException(Utils::ERROR_INVALID_ARGUMENT, "Invalid ReadPreference");
				break;
		}
	}
}

<<__NativeData("MongoDBDriverServer")>>
final class Server {
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
/* }}} */

/* {{{ BSON and Serialization Classes */
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
/* }}} */
