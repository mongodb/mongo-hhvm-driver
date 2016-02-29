<?hh
namespace MongoDB\Driver;

final class WriteConcernError {
	private $code;
	private $message;
	private $info;

	private function __construct()
	{
		throw new Exception\RunTimeException("Accessing private constructor");
	}

	public function getCode() : int
	{
		return $this->code;
	}

	public function getMessage() : string
	{
		return $this->message;
	}

	public function getInfo() : ?array
	{
		return $this->info;
	}

	public function __debugInfo() : array
	{
		return [
			'message' => $this->message,
			'code' => $this->code,
			'info' => $this->info,
		];
	}

}

final class WriteError {
	private $message;
	private $code;
	private $index;
	private $info;

	private function __construct()
	{
		throw new Exception\RunTimeException("Accessing private constructor");
	}

	public function getMessage()
	{
		return $this->message;
	}

	public function getCode()
	{
		return $this->code;
	}

	public function getIndex()
	{
		return $this->index;
	}

	public function getInfo()
	{
		return $this->info;
	}

	public function __debugInfo()
	{
		return [
			'message' => $this->message,
			'code' => $this->code,
			'index' => $this->index,
			'info' => $this->info
		];
	}
}

<<__NativeData("MongoDBDriverWriteResult")>>
final class WriteResult {
	private $nUpserted = 0;
	private $nMatched = 0;
	private $nRemoved = 0;
	private $nInserted = 0;
	private $nModified = 0;
	private $upsertedIds = null;
	private $writeErrors = [];
	private $writeConcernError = NULL;
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

	<<__Native>>
	public function getServer() : Server;

	public function getUpsertedIds(): array
	{
		if ($this->upsertedIds && gettype($this->upsertedIds) == 'array') {
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

	public function getWriteConcernError()
	{
		if ($this->writeConcernError && gettype($this->writeConcernError) == 'object') {
			return $this->writeConcernError;
		}
		return null;
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

	public function __debugInfo() : array
	{
		$ret = [];

		$ret['nInserted'] = $this->nInserted;
		$ret['nMatched'] = $this->nMatched;
		if ($this->omit_nModified) {
			$ret['nModified'] = null;
		} else {
			$ret['nModified'] = $this->nModified;
		}
		$ret['nRemoved'] = $this->nRemoved;
		$ret['nUpserted'] = $this->nUpserted;

		$ret['upsertedIds'] = (array) $this->upsertedIds;
		$ret['writeErrors'] = $this->writeErrors;
		$ret['writeConcernError'] = $this->writeConcernError;

		if ($this->writeConcern) {
			$ret['writeConcern'] = $this->writeConcern;
		} else {
			$ret['writeConcern'] = NULL;
		}

		return $ret;
	}
}

<<__NativeData("MongoDBDriverManager")>>
class Manager {
	<<__Native>>
	public function __construct(string $dsn = "mongodb://localhost", array $options = array(), array $driverOptions = array());

	<<__Native>>
	public function __debugInfo() : array;

	<<__Native>>
	public function executeCommand(string $db, Command $command, ReadPreference $readPreference = null): Cursor;

	<<__Native>>
	public function executeQuery(string $namespace, Query $query, ReadPreference $readPreference = null): Cursor;

	<<__Native>>
	public function executeBulkWrite(string $namespace, BulkWrite $bulk, WriteConcern $writeConcern = null): WriteResult;

	<<__Native>>
	public function getServers(): array;

	<<__Native>>
	public function getReadConcern() : MongoDB\Driver\ReadConcern;

	<<__Native>>
	public function getReadPreference() : MongoDB\Driver\ReadPreference;

	<<__Native>>
	public function getWriteConcern() : MongoDB\Driver\WriteConcern;

	<<__Native>>
	public function __wakeUp() : void;

	<<__Native>>
	public function selectServer(ReadPreference $readPreference): Server;
}

class Utils {
	const ERROR_INVALID_ARGUMENT  = 1;
	const ERROR_RUNTIME           = 2;
	const ERROR_MONGOC_FAILED     = 3;
	const ERROR_WRITE_FAILED      = 4;
	const ERROR_CONNECTION_FAILED = 5;

	static public function throwHippoException($domain, $message)
	{
		switch ($domain) {
			case self::ERROR_INVALID_ARGUMENT:
				throw new \MongoDB\Driver\Exception\InvalidArgumentException($message);

			case self::ERROR_RUNTIME:
			case self::ERROR_MONGOC_FAILED:
				throw new Exception\RuntimeException($mssage);

			case self::ERROR_WRITE_FAILED:
				throw new Exception\WriteException($message);

			case self::ERROR_CONNECTION_FAILED:
				throw new Exception\ConnectionException($message);
		}
	}

	static public function mustBeArrayOrObject(string $name, mixed $value, string $context = '')
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
	private function __construct(string $id)
	{
		throw new Exception\RunTimeException("Accessing private constructor");
	}

	<<__Native>>
	public function __debugInfo() : array;

	<<__Native>>
	public function __toString() : string;
}

<<__NativeData("MongoDBDriverCursor")>>
final class Cursor implements Traversable, Iterator {
	private function __construct(Server $server, CursorId $cursorId, array $firstBatch)
	{
		throw new Exception\RunTimeException("Accessing private constructor");
	}

	<<__Native>>
	public function __debugInfo() : array;

	<<__Native>>
	public function getId() : CursorId;

	<<__Native>>
	public function getServer() : Server;

	<<__Native>>
	public function isDead() : bool;

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
		$this->command = (object) $command;
	}

	public function __debugInfo()
	{
		return [ 'command' => $this->command ];
	}
}

final class Query {
	private array $query;

	public function __construct(mixed $filter, array $options = array())
	{
		$zquery = [];

		/* phongo_query_init */
		Utils::mustBeArrayOrObject('parameter 1', $filter, "MongoDB\Driver\Query::__construct");

		if ($options) {
			$this->query['batchSize'] = array_key_exists('batchSize', $options ) ? (int) $options['batchSize'] : 0;
			$this->query['flags'] = array_key_exists('flags', $options ) ? (int) $options['flags'] : 0;
			$this->query['limit'] = array_key_exists('limit', $options ) ? (int) $options['limit'] : 0;
			$this->query['skip'] = array_key_exists('skip', $options ) ? (int) $options['skip'] : 0;

			if (array_key_exists('readConcern', $options)) {
				if (!($options['readConcern'] instanceof \MongoDB\Driver\readConcern)) {
					throw new \MongoDB\Driver\Exception\InvalidArgumentException(
						'Expected "readConcern" option to be MongoDB\Driver\ReadConcern, ' .
						gettype($options['readConcern']) . ' given'
					);
				} else {
					if ($options['readConcern']->getLevel() != NULL) {
						$this->query['readConcern'] = $options['readConcern']->getLevel();
					}
				}
			}

			if (array_key_exists('modifiers', $options)) {
				Utils::mustBeArrayOrObject('modifiers', $options['modifiers']);
				foreach ($options['modifiers'] as $key => $value) {
					$this->query['query'][$key] = $value;
				}
			}

			if (array_key_exists('projection', $options)) {
				Utils::mustBeArrayOrObject('projection', $options['projection']);
				$this->query['fields'] = (array) $options['projection'];
			}

			if (array_key_exists('sort', $options)) {
				Utils::mustBeArrayOrObject('sort', $options['sort']);
				$this->query['query']['$orderby'] = (object) $options['sort'];
			}
		}

		$this->query['query']['$query'] = (object) $filter;
	}

	public function __debugInfo() : Array
	{
		return [
			'query' => (object) $this->query['query'],
			'selector' => array_key_exists('fields', $this->query) ? (object) $this->query['fields'] : NULL,
			'flags' => $this->query['flags'],
			'skip' => $this->query['skip'],
			'limit' => $this->query['limit'],
			'batch_size' => $this->query['batchSize'],
			'readConcern' => array_key_exists('readConcern', $this->query) ? [ 'level' => $this->query['readConcern'] ] : NULL,
		];
	}
}

<<__NativeData("MongoDBDriverBulkWrite")>>
final class BulkWrite implements \Countable {
	<<__Native>>
	public function __construct(?array $bulkWriteOptions = array());

	<<__Native>>
	public function insert(mixed $document) : mixed;

	<<__Native>>
	public function update(mixed $query, mixed $newObj, ?array $updateOptions = array()) : void;

	<<__Native>>
	public function delete(mixed $query, ?array $deleteOptions = array()) : void;

	<<__Native>>
	public function count() : int;

	<<__Native>>
	public function __debugInfo() : array;
}


<<__NativeData("MongoDBDriverReadConcern")>>
final class ReadConcern {
	<<__Native>>
	public function __construct(?string $level = NULL) : void;

	<<__Native>>
	public function getLevel() : mixed;

	<<__Native>>
	public function __debugInfo() : array;
}

<<__NativeData("MongoDBDriverReadPreference")>>
final class ReadPreference {
	<<__Native>>
	private function _setReadPreference(int $readPreference): void;

	<<__Native>>
	private function _setReadPreferenceTags(array $tagSets): void;

	public function __construct(int $readPreference, array $tagSets = null)
	{
		if ($tagSets !== NULL && gettype($tagSets) != 'array') {
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
				Utils::throwHippoException(Utils::ERROR_INVALID_ARGUMENT, "Invalid mode: " . $readPreference);
				break;
		}
	}

	<<__Native>>
	public function getMode() : int;

	<<__Native>>
	public function getTagSets() : array;

	<<__Native>>
	public function __debugInfo() : array;
}

<<__NativeData("MongoDBDriverServer")>>
final class Server {
	private $__serverId = NULL;

	private function __construct()
	{
		throw new Exception\RunTimeException("Accessing private constructor");
	}

	<<__Native>>
	public function __debugInfo() : array;

	<<__Native>>
	public function getHost(): string;

	<<__Native>>
	final public function getInfo(): array;

	<<__Native>>
	public function getLatency() : int;

	<<__Native>>
	public function getPort(): int;

	<<__Native>>
	public function getTags() : array;

	<<__Native>>
	public function getType(): int;

	<<__Native>>
	public function isPrimary() : bool;

	<<__Native>>
	public function isSecondary() : bool;

	<<__Native>>
	public function isArbiter() : bool;

	<<__Native>>
	public function isHidden() : bool;

	<<__Native>>
	public function isPassive() : bool;

	<<__Native>>
	public function executeBulkWrite(string $namespace, BulkWrite $bulk, WriteConcern $writeConcern = null): WriteResult;

	<<__Native>>
	public function executeCommand(string $db, Command $command, ReadPreference $readPreference = null): Cursor;

	<<__Native>>
	public function executeQuery(string $namespace, Query $query, ReadPreference $readPreference = null): Cursor;
}

<<__NativeData("MongoDBDriverWriteConcern")>>
final class WriteConcern {
	<<__Native>>
	public function __construct(mixed $w, ?integer $wtimeout = 0, ?boolean $journal = NULL);

	<<__Native>>
	public function getJournal() : mixed;

	<<__Native>>
	public function getW() : mixed;

	<<__Native>>
	public function getWtimeout() : int;

	<<__Native>>
	public function __debugInfo() : array;
}
/* }}} */


/* {{{ Exception Classes */
namespace MongoDB\Driver\Exception;

interface Exception {}

class ConnectionException extends RuntimeException {}

class AuthenticationException extends ConnectionException {}
class ConnectionTimeoutException extends ConnectionException {}
class ExecutionTimeoutException extends RuntimeException {}
class InvalidArgumentException extends \InvalidArgumentException implements Exception {}
class LogicException extends \LogicException implements Exception {}
class RuntimeException extends \RunTimeException implements Exception {}
class SSLConnectionException extends ConnectionException {}
class UnexpectedValueException extends \UnexpectedValueException implements Exception {}
class BulkWriteException extends WriteException {}
abstract class WriteException extends RunTimeException
{
	protected $writeResult = null;

	final public function getWriteResult() : \MongoDB\Driver\WriteResult
	{
		if (func_num_args() != 0) {
			trigger_error(
				sprintf(
					"%s() expects exactly 0 parameters, %d given in %s on line %d",
					"MongoDB\Driver\Exception\WriteException::getWriteResult",
					func_num_args(),
					__FILE__,
					__LINE__
				),
				E_WARNING
			);
		}
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

trait DenySerialization
{
	public function serialize() : string
	{
		$name = get_class( $this );
		throw new \Exception("Serialization of '{$name}' is not allowed");
	}

	public function unserialize(mixed $data) : void
	{
		$name = get_class( $this );
		throw new \Exception("Unserialization of '{$name}' is not allowed");
	}
}

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

final class Binary implements Type, \Serializable
{
	use DenySerialization;

	public function __construct(private string $data, private int $type)
	{
	}

	public function getType()
	{
		$func_args = func_num_args();
		if ($func_args != 0) {
			trigger_error("MongoDB\BSON\Binary::getType() expects exactly 0 parameters, {$func_args} given", E_WARNING);
			return NULL;
		}
		return $this->type;
	}

	public function getData() : string
	{
		return $this->data;
	}

	<<__Native>>
	function __debugInfo() : array;
}

final class Javascript implements Type, \Serializable
{
	use DenySerialization;

	public function __construct(private string $code, private ?mixed $scope = NULL)
	{
	}

	public function __debugInfo() : array
	{
		return [
			'javascript' => $this->code,
			'scope' => (object) $this->scope
		];
	}
}

final class MaxKey implements Type, \Serializable
{
	use DenySerialization;
}

final class MinKey implements Type, \Serializable
{
	use DenySerialization;
}

<<__NativeData("MongoDBBsonObjectID")>>
final class ObjectID implements Type, \Serializable
{
	use DenySerialization;

	<<__Native>>
	public function __construct(string $objectId = null);

	<<__Native>>
	public function __toString() : string;

	<<__Native>>
	public function __debugInfo() : array;
}

final class Regex implements Type, \Serializable
{
	use DenySerialization;

	public function __construct(private string $pattern, private string $flags)
	{
	}

	public function getPattern() : string
	{
		return $this->pattern;
	}

	public function getFlags() : string
	{
		return $this->flags;
	}

	public function __toString() : string
	{
		return "/{$this->pattern}/{$this->flags}";
	}

	public function __debugInfo() : array
	{
		return [
			'pattern' => $this->pattern,
			'flags' => $this->flags
		];
	}
}

final class Timestamp implements Type, \Serializable
{
	use DenySerialization;

	public function __construct(private int $increment, private int $timestamp)
	{
		if ( $increment < 0 || $increment > 4294967295 )
		{
			throw new \MongoDB\Driver\Exception\InvalidArgumentException( "Expected increment to be an unsigned 32-bit integer, {$increment} given" );
		}
		if ( $timestamp < 0 || $timestamp > 4294967295 )
		{
			throw new \MongoDB\Driver\Exception\InvalidArgumentException( "Expected timestamp to be an unsigned 32-bit integer, {$timestamp} given" );
		}
	}

	public function __toString() : string
	{
		return sprintf( "[%d:%d]", $this->increment, $this->timestamp );
	}

	public function __debugInfo() : array
	{
		return [
			'increment' => $this->increment,
			'timestamp' => $this->timestamp
		];
	}
}

final class UTCDateTime implements Type, \Serializable
{
	use DenySerialization;

	private int $milliseconds;

	<<__Native>>
	public function __construct(mixed $milliseconds);

	public function __toString() : string
	{
		return (string) $this->milliseconds;
	}

	<<__Native>>
	public function toDateTime() : \DateTime;

	public function __debugInfo() : array
	{
		return [ 'milliseconds' => $this->milliseconds ];
	}
}

/* }}} */
