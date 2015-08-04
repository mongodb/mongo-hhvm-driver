<?hh
namespace MongoDB\Driver;

<<__NativeData("MongoDBDriverManager")>>
class Manager {
	<<__Native>>
	function __construct(string $dsn = "localhost", array $options = array(), array $driverOptions = array());

	<<__Native>>
	function executeCommand(string $db, Command $command, ReadPreference $readPreference = null): Cursor;

	<<__Native>>
	function executeQuery(string $namespace, Query $query, ReadPreference $readPreference = null): Cursor;

	<<__Native>>
	function executeBulkWrite(string $namespace, BulkWrite $bulk, WriteConcern $writeConcern = null): WriteResult;

	<<__Native>>
	function executeInsert(string $namespace, mixed $document, ?WriteConcern $writeConcern = null): WriteResult;

	<<__Native>>
	function executeUpdate(string $namespace, mixed $query, mixed $newObj, ?array $updateOptions = array(), ?WriteConcern $writeConcern = null): WriteResult;

	<<__Native>>
	function executeDelete(string $namespace, mixed $query, ?array $deleteOptions = array(), WriteConcern $writeConcern = null): WriteResult;

	function getServers(): void
	{
	}

	<<__Native>>
	function selectServer(ReadPreference $readPreference): Server;
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
				throw new Exception\RuntimeException($mssage);

			case self::ERROR_WRITE_FAILED:
				throw new Exception\WriteException($message);

			case self::ERROR_CONNECTION_FAILED:
				throw new Exception\ConnectionException($message);
		}
	}

	static function mustBeArrayOrObject(string $name, mixed $value, string $context = '')
	{
		$valueType = gettype($value);
		if (!in_array($valueType, [ 'array', 'object' ])) {
			Utils::throwHippoException(
				Utils::ERROR_INVALID_ARGUMENT,
				($context != '' ? "{$context}() expects" : 'Expected') . " {$name} to be array or object, {$valueType} given"
			);
		}
	}
}

/* {{{ Cursor Classes */
<<__NativeData("MongoDBDriverCursorId")>>
final class CursorId {
	<<__Native>>
	public function __construct(string $id);

	<<__Native>>
	public function __toString() : string;
}

<<__NativeData("MongoDBDriverCursor")>>
final class Cursor implements Traversable, Iterator {
	public function __construct(Server $server, CursorId $cursorId, array $firstBatch)
	{
	}

	<<__Native>>
	public function getId() : CursorId;

	<<__Native>>
	public function getServer() : Server;

	public function isDead() : bool
	{
		throw new \Exception("isDead is not implemented yet");
	}

	/**
	* Get the current element
	*
	* @return ReturnType -
	*/
	<<__Native>>
	public function current(): mixed;

	/**
	* Get the current key
	*
	* @return ReturnType -
	*/
	<<__Native>>
	public function key(): int;

	/**
	* Move forward to the next element
	*
	* @return ReturnType -
	*/
	<<__Native>>
	public function next(): mixed;

	/**
	* Rewind the iterator to the first element
	*
	* @return ReturnType -
	*/
	<<__Native>>
	public function rewind(): void;

	/**
	* Check if current position is valid
	*
	* @return ReturnType -
	*/
	<<__Native>>
	public function valid(): bool;

	<<__Native>>
	public function toArray(): array;

	<<__Native>>
	public function setTypeMap(array $typemap): void;
}
/* }}} */

/* {{{ Value Classes */
final class Command {
	private array $command;

	public function __construct(mixed $command)
	{
		$this->command = $command;
	}
}

final class Query {
	private array $query;

	public function __construct(mixed $filter, array $options = array())
	{
		$zquery = [];

		/* phongo_query_init */
		Utils::mustBeArrayOrObject('filter', $filter, "MongoDB\Driver\Query::__construct");

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

<<__NativeData("MongoDBDriverBulkWrite")>>
final class BulkWrite implements \Countable {
	<<__Native>>
	function __construct(?boolean $ordered = true);

	<<__Native>>
	function insert(mixed $document) : \MongoDB\BSON\ObjectID;

	<<__Native>>
	function update(mixed $query, mixed $newObj, ?array $updateOptions = array()) : void;

	<<__Native>>
	function delete(mixed $query, ?array $deleteOptions = array()) : void;

	<<__Native>>
	function count() : int;

	<<__Native>>
	function __debugInfo() : array;
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
	<<__Native>>
	final public function getInfo(): array;

	<<__Native>>
	function executeCommand(string $db, Command $command, ReadPreference $readPreference = null): Cursor;

	<<__Native>>
	function executeQuery(string $namespace, Query $query, ReadPreference $readPreference = null): Cursor;
}

<<__NativeData("MongoDBDriverWriteResult")>>
final class WriteResult {
	private $nUpserted = 0;
	private $nMatched = 0;
	private $nRemoved = 0;
	private $nInserted = 0;
	private $nModified = 0;
	private $upsertedIds = null;
	private $writeErrors = null;
	private $writeConcernError = null;
	private $info = null;

	private function __construct()
	{
		throw new Exception\RunTimeException("Accessing private constructor");
	}

	public function __wakeup()
	{
		throw new Exception\RunTimeException("MongoDB\\Driver objects cannot be serialized");
	}

	public function getInsertedCount() { return $this->nInserted; }
	public function getMatchedCount()  { return $this->nMatched; }
	public function getModifiedCount() { return $this->nModified; }
	public function getDeletedCount()  { return $this->nRemoved; }
	public function getUpsertedCount() { return $this->nUpserted; }

	public function getInfo()
	{
		if ($this->info && gettype($this->info) == 'array') {
			return $this->info;
		}
		return [];
	}

	<<__Native>>
	public function getServer() : Server;

	public function getUpsertedIds(): array
	{
		if ($this->upsertedIds && gettype($this->upsertedIds) == 'object') {
			$upsertedIds = [];
			
			foreach( (array) $this->upsertedIds as $idDoc )
			{
				$idDoc = (array) $idDoc;
				$upsertedIds[$idDoc['index']] = $idDoc['_id'];
			}

			return $upsertedIds;
		}
		return [];
	}

	public function getWriteConcernError(): WriteConcernError
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

	<<__Native>>
	public function isAcknowledged() : bool;
}
/* }}} */


/* {{{ Exception Classes */
namespace MongoDB\Driver\Exception;

interface Exception {}

class ConnectionException extends RuntimeException {}

class AuthenticationException extends ConnectionException {}
class BulkWriteException extends WriteException {}
final class ConnectionTimeoutException extends ConnectionException {}
final class DuplicateKeyException extends WriteException {}
final class ExecutionTimeoutException extends RuntimeException {}
class InvalidArgumentException extends \InvalidArgumentException implements Exception {}
class LogicException extends \LogicException implements Exception {}
class RuntimeException extends \RunTimeException implements Exception {}
final class SSLConnectionException extends ConnectionException {}
class UnexpectedValueException extends \UnexpectedValueException implements Exception {}
class WriteConcernException extends WriteException {}
class WriteException extends RunTimeException
{
	protected $writeResult = null;

	final public function getWriteResult() : \MongoDB\Driver\WriteResult
	{
		return $this->writeResult;
	}
}

/* }}} */


/* {{{ BSON and Serialization Classes */
namespace MongoDB\BSON;

<<__Native>>
function fromPHP(mixed $data) : string;

<<__Native>>
function fromJson(string $data) : mixed;

<<__Native>>
function toPHP(string $data, ?array $typemap = array()) : mixed;

<<__Native>>
function toJson(string $data) : mixed;

interface Type
{
}

interface Serializable extends Type
{
	function bsonSerialize() : array;
}

interface Unserializable
{
	function bsonUnserialize(array $data) : void;
}

interface Persistable extends Serializable, Unserializable
{
}

class Binary implements Type
{
	function __construct(private string $data, private int $type)
	{
	}

	function getType()
	{
		$func_args = func_num_args();
		if ($func_args != 0) {
			trigger_error("MongoDB\BSON\Binary::getType() expects exactly 0 parameters, {$func_args} given", E_WARNING);
			return NULL;
		}
		return $this->type;
	}

	function getData() : string
	{
		return $this->data;
	}

	<<__Native>>
	function __debugInfo() : array;
}

class Javascript implements Type
{
	function __construct(private string $code, private ?mixed $scope = NULL)
	{
	}
}

class MaxKey implements Type
{
}

class MinKey implements Type
{
}

<<__NativeData("MongoDBBsonObjectID")>>
class ObjectID implements Type
{
	<<__Native>>
	function __construct(string $objectId = null);

	<<__Native>>
	function __toString() : string;

	<<__Native>>
	function __debugInfo() : array;
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

class Timestamp implements Type
{
	function __construct(private int $increment, private int $timestamp)
	{
	}

	function __toString() : string
	{
		return sprintf( "[%d:%d]", $this->increment, $this->timestamp );
	}
}

class UTCDateTime implements Type
{
	private int $milliseconds;

	<<__Native>>
	function __construct(mixed $milliseconds);

	function __toString() : string
	{
		return (string) $this->milliseconds;
	}

	<<__Native>>
	function toDateTime() : \DateTime;
}

/* }}} */
