<?hh
namespace MongoDB\BSON;

interface Type
{
}

interface Serializable extends Type
{
	function bsonSerialize() : mixed;
}

interface Unserializable
{
	function bsonUnserialize(array $data) : void;
}

interface Persistable extends Serializable, Unserializable
{
}

/* == MONITORING ============================================================= */
namespace MongoDB\Driver\Monitoring;

/* ------ Building blocks for value obejcts */
abstract class _CommandEvent
{
	private string                 $commandName;
	private \MongoDB\Driver\Server $server;
	private integer                $operationId;
	private integer                $requestId;

	private function __construct()
	{
		throw new \MongoDB\Driver\Exception\RunTimeException("Accessing private constructor");
	}

	public function getCommandName() : string
	{
		return $this->commandName;
	}

	// libmongoc use getHost, and the spec uses "connectionId".
	public function getServer() : \MongoDB\Driver\Server
	{
		return $this->server;
	}

	// technically int64_t
	public function getOperationId() : string
	{
		return (string) $this->operationId;
	}

	// technically int64_t
	public function getRequestId() : string
	{
		return (string) $this->requestId;
	}
}

abstract class _CommandResultEvent extends _CommandEvent
{
	private integer $durationMicros;

	// spec is unitless, libmongoc does microseconds. Spec suggests "Nanos" as
	// another alternative. (technically int64_t, but 2147 sec on 32bit platforms
	// ought to be enough)
	public function getDurationMicros() : integer
	{
		return $this->durationMicros;
	}
}

/* ------ Value Objects ------------------- */
final class CommandStartedEvent extends _CommandEvent
{
	private mixed $command;
	private string $databaseName;

	// raw command document, with default BSON conversion rules
	public function getCommand() : mixed
	{
		return $this->command;
	}

	public function getDatabaseName() : string
	{
		return $this->databaseName;
	}

	public function __debugInfo() : array
	{
		return [
			'command' => $this->getCommand(),
			'commandName' => $this->getCommandName(),
			'databaseName' => $this->getDatabaseName(),
			'operationId' => $this->getOperationId(),
			'requestId' => $this->getRequestId(),
			'server' => $this->getServer(),
		];
	}
}

final class CommandSucceededEvent extends _CommandResultEvent
{
	private mixed $reply;

	public function getReply() : mixed
	{
		return $this->reply;
	}

	public function __debugInfo() : array
	{
		return [
			'commandName' => $this->getCommandName(),
			'durationMicros' => $this->getDurationMicros(),
			'operationId' => $this->getOperationId(),
			'reply' => $this->getReply(),
			'requestId' => $this->getRequestId(),
			'server' => $this->getServer(),
		];
	}
}

final class CommandFailedEvent extends _CommandResultEvent
{
	private object $error;

	// Spec calls it failure, libmongoc returns a bson_error_t.
	public function getError() : \MongoDB\Driver\Exception\Exception
	{
		return $this->error;
	}

	public function __debugInfo() : array
	{
		return [
			'commandName' => $this->getCommandName(),
			'durationMicros' => $this->getDurationMicros(),
			'error' => $this->getError(),
			'operationId' => $this->getOperationId(),
			'requestId' => $this->getRequestId(),
			'server' => $this->getServer(),
		];
	}
}

/* ------ Subscriber Interface ------------ */
interface Subscriber {}

interface CommandSubscriber extends Subscriber
{
	public function commandStarted( \MongoDB\Driver\Monitoring\CommandStartedEvent $event );
	public function commandSucceeded( \MongoDB\Driver\Monitoring\CommandSucceededEvent $event );
	public function commandFailed( \MongoDB\Driver\Monitoring\CommandFailedEvent $event );
}

/* =========================================================================== */

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
		$ret['nModified'] = $this->nModified;
		$ret['nRemoved'] = $this->nRemoved;
		$ret['nUpserted'] = $this->nUpserted;

		$ret['upsertedIds'] = (array) $this->upsertedIds;
		$ret['writeErrors'] = $this->writeErrors;
		if (is_object($this->writeConcernError) && $this->writeConcernError instanceof \MongoDB\Driver\WriteConcernError) {
			$ret['writeConcernError'] = $this->writeConcernError;
		} else {
			$ret['writeConcernError'] = NULL;
		}

		if (is_object($this->writeConcern) && $this->writeConcern instanceof \MongoDB\Driver\WriteConcern) {
			$ret['writeConcern'] = $this->writeConcern;
		} else {
			$ret['writeConcern'] = NULL;
		}

		return $ret;
	}
}

<<__NativeData("MongoDBDriverManager")>>
final class Manager {
	<<__Native>>
	public function __construct(string $dsn = "", array $options = array(), array $driverOptions = array());

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
	public function __wakeup() : void;

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
	private mixed $filter = NULL;
	private array $opts = [];
	private string $readConcernLevel = NULL;

	/* Helpers for setting / validating scalar options */
	private function _queryOptBool( string $optName, array $options, string $key )
	{
		if ( array_key_exists( $key, $options ) )
		{
			$this->opts[$optName] = (bool) $options[$key];
			return true;
		}

		return false;
	}
	
	private function _queryOptDocument( string $optName, array $options, string $key )
	{
		if ( array_key_exists( $key, $options ) )
		{
			if ( !is_array( $options[$key] ) && !is_object( $options[$key] ) )
			{
				throw new \MongoDB\Driver\Exception\InvalidArgumentException(
					sprintf(
						'Expected "%s" %s to be array or object, %s given',
						$key, $key[0] == '$' ? 'modifier' : 'option', gettype( $options[$key] )
					)
				);
			}
			$this->opts[$optName] = (object) $options[$key];
			return true;
		}

		return false;
	}

	private function _queryOptInt64( string $optName, array $options, string $key )
	{
		if ( array_key_exists( $key, $options ) )
		{
			$this->opts[$optName] = (int) $options[$key];
			return true;
		}

		return false;
	}

	private function _queryOptString( string $optName, array $options, string $key )
	{
		if ( array_key_exists( $key, $options ) )
		{
			if ( !is_string( $options[$key] ) )
			{
				throw new \MongoDB\Driver\Exception\InvalidArgumentException(
					sprintf(
						'Expected "%s" %s to be string, %s given',
						$key, $key[0] == '$' ? 'modifier' : 'option', gettype( $options[$key] )
					)
				);
			}
			$this->opts[$optName] = $options[$key];
			return true;
		}

		return false;
	}

	/* Helpers for setting / validating complex options */
	private function _queryInitHint( array $options, array $modifiers )
	{
		if ( array_key_exists( 'hint', $options ) )
		{
			if ( is_string( $options['hint'] ) )
			{
				$this->_queryOptString( 'hint', $options, 'hint' );
			}
			else if ( is_object( $options['hint'] ) || is_array( $options['hint'] ) )
			{
				$this->_queryOptDocument( 'hint', $options, 'hint' );
			}
			else
			{
				throw new \MongoDB\Driver\Exception\InvalidArgumentException(
					sprintf(
						'Expected "hint" option to be string, array, or object, %s given',
						gettype( $options['hint'] )
					)
				);
			}
		}
		else if ( array_key_exists( '$hint', $modifiers ) )
		{
			if ( is_string( $modifiers['$hint'] ) )
			{
				$this->_queryOptString( 'hint', $modifiers, '$hint' );
			}
			else if ( is_object( $modifiers['$hint'] ) || is_array( $modifiers['$hint'] ) )
			{
				$this->_queryOptDocument( 'hint', $modifiers, '$hint' );
			}
			else
			{
				throw new \MongoDB\Driver\Exception\InvalidArgumentException(
					sprintf(
						'Expected "$hint" modifier to be string, array, or object, %s given',
						gettype( $modifiers['$hint'] )
					)
				);
			}
		}
	}

	private function _queryInitLimitAndSingleBatch( array $options )
	{
		if ( array_key_exists( 'limit', $options ) && $options['limit'] < 0 )
		{
			$this->opts['limit'] = 0 - (int) $options['limit'];

			if ( array_key_exists( 'singleBatch', $options ) && !$options['singleBatch'] )
			{
				throw new \MongoDB\Driver\Exception\InvalidArgumentException( 'Negative "limit" option conflicts with false "singleBatch" option' );
			}
			else
			{
				$this->opts['singleBatch'] = true;
			}
		}
		else
		{
			$this->_queryOptInt64( 'limit', $options, 'limit' );
			$this->_queryOptBool( 'singleBatch', $options, 'singleBatch' );
		}
	}

	private function _queryInitReadConcern( array $options )
	{
		if ( array_key_exists( 'readConcern', $options ) )
		{
			if ( !is_object( $options['readConcern'] ) || ! $options['readConcern'] instanceof \MongoDB\Driver\ReadConcern )
			{
				throw new \MongoDB\Driver\Exception\InvalidArgumentException(
					sprintf(
						'Expected "readConcern" option to be %s, %s given',
						'MongoDB\Driver\ReadConcern',
						gettype( $options['readConcern'] )
					)
				);
			}

			$this->readConcernLevel = $options['readConcern']->getLevel();
		}
	}

	public function __construct(mixed $filter, array $options = array())
	{
		$modifiers = [];

		Utils::mustBeArrayOrObject('parameter 1', $filter, "MongoDB\Driver\Query::__construct");

		$this->filter = $filter;

		if ( count( $options ) == 0 )
		{
			return;
		}

		if ( array_key_exists( 'modifiers', $options ) )
		{
			$modifiers = $options['modifiers'];

			if ( !is_array( $modifiers ) )
			{
				throw new \MongoDB\Driver\Exception\InvalidArgumentException(
					sprintf(
						'Expected "modifiers" option to be array, %s given',
						gettype( $options['modifiers'] )
					)
				);
			}
		}

		$this->_queryOptBool( 'allowPartialResults', $options, 'allowPartialResults' )
			|| $this->_queryOptBool( 'allowPartialResults', $options, 'partial' );
		$this->_queryOptBool( 'awaitData', $options, 'awaitData' );
		$this->_queryOptInt64( 'batchSize', $options, 'batchSize' );
		$this->_queryOptDocument( 'collation', $options, 'collation' );
		$this->_queryOptString( 'comment', $options, 'comment' )
			|| $this->_queryOptString( 'comment', $modifiers, '$comment' );
		$this->_queryOptBool( 'exhaust', $options, 'exhaust' );
		$this->_queryOptDocument( 'max', $options, 'max' )
			|| $this->_queryOptDocument( 'max', $modifiers, '$max' );
		$this->_queryOptInt64( 'maxScan', $options, 'maxScan' )
			|| $this->_queryOptInt64( 'maxScan', $modifiers, '$maxScan' );
		$this->_queryOptInt64( 'maxTimeMS', $options, 'maxTimeMS' )
			|| $this->_queryOptInt64( 'maxTimeMS', $modifiers, '$maxTimeMS' );
		$this->_queryOptDocument( 'min', $options, 'min' )
			|| $this->_queryOptDocument( 'min', $modifiers, '$min' );
		$this->_queryOptBool( 'noCursorTimeout', $options, 'noCursorTimeout' );
		$this->_queryOptBool( 'oplogReplay', $options, 'oplogReplay' );
		$this->_queryOptDocument( 'projection', $options, 'projection' );
		$this->_queryOptBool( 'returnKey', $options, 'returnKey' )
			|| $this->_queryOptBool( 'returnKey', $modifiers, '$returnKey' );
		$this->_queryOptBool( 'showRecordId', $options, 'showRecordId' )
			|| $this->_queryOptBool( 'showRecordId', $modifiers, '$showDiskLoc' );
		$this->_queryOptInt64( 'skip', $options, 'skip' );
		$this->_queryOptDocument( 'sort', $options, 'sort' )
			|| $this->_queryOptDocument( 'sort', $modifiers, '$orderby' );
		$this->_queryOptBool( 'snapshot', $options, 'snapshot' )
			|| $this->_queryOptBool( 'snapshot', $modifiers, '$snapshot' );
		$this->_queryOptBool( 'tailable', $options, 'tailable' );

		$this->_queryOptBool( 'explain', $modifiers, '$explain' );

		$this->_queryInitHint( $options, $modifiers );
		$this->_queryInitLimitAndSingleBatch( $options );
		$this->_queryInitReadConcern( $options );
	}

	public function __debugInfo() : Array
	{
		return [
			'filter' => (object) $this->filter,
			'options' => (object) $this->opts,
			'readConcern' => $this->readConcernLevel ? [ 'level' => $this->readConcernLevel ] : NULL,
		];
	}
}

<<__NativeData("MongoDBDriverBulkWrite")>>
final class BulkWrite implements \Countable {
	<<__Native>>
	public function __construct(?array $bulkWriteOptions = array());

	<<__Native>>
	public function insert(mixed $document) : mixed;

	private function _queryOptBool( array &$transformedOptions, array $options, string $key )
	{
		if ( array_key_exists( $key, $options ) )
		{
			$transformedOptions[$key] = (bool) $options[$key];
		}
	}

	private function _queryOptDocument( array &$transformedOptions, array $options, string $key )
	{
		if ( array_key_exists( $key, $options ) )
		{
			if ( !is_array( $options[$key] ) && !is_object( $options[$key] ) )
			{
				throw new \MongoDB\Driver\Exception\InvalidArgumentException(
					sprintf(
						'Expected "%s" option to be array or object, %s given',
						$key, gettype( $options[$key] )
					)
				);
			}

			$transformedOptions[$key] = (object) $options[$key];
		}
	}

	private function _transformUpdateOptions( array $options = [] ) : array
	{
		$transformedOptions = [];

		$this->_queryOptBool( $transformedOptions, $options, 'multi' );
		$this->_queryOptBool( $transformedOptions, $options, 'upsert' );
		$this->_queryOptDocument( $transformedOptions, $options, 'collation' );

		return $transformedOptions;
	}

	private function _updateHasOperators( array $update ) : bool
	{
		foreach ( $update as $key => $value )
		{
			if ( $key[0] == '$' )
			{
				return true;
			}
		}

		return false;
	}

	public function update(mixed $query, mixed $update, array $options = []) : void
	{
		/* Can throw an exception */
		$updateOptions = $this->_transformUpdateOptions( $options );

		$hasOperators = $this->_updateHasOperators( (array) $update );

		return $this->_update( $hasOperators, $query, $update, $updateOptions );
	}

	<<__Native>>
	private function _update(bool $hasOperators, mixed $query, mixed $update, array $options = []) : void;

	private function _transformDeleteOptions( array $options = [] ) : array
	{
		$transformedOptions = [ 'limit' => 0 ];

		if ( array_key_exists( 'limit', $options ) )
		{
			$transformedOptions['limit'] = (int) ($options['limit'] ? 1 : 0);
		}
		$this->_queryOptDocument( $transformedOptions, $options, 'collation' );

		return $transformedOptions;
	}

	public function delete(mixed $query, array $options = []) : void
	{
		/* Can throw an exception */
		$deleteOptions = $this->_transformDeleteOptions( $options );

		return $this->_delete( $query, $deleteOptions );
	}

	<<__Native>>
	public function _delete(mixed $query, array $options = []) : void;

	<<__Native>>
	public function count() : int;

	<<__Native>>
	public function __debugInfo() : array;
}


<<__NativeData("MongoDBDriverReadConcern")>>
final class ReadConcern implements \MongoDB\BSON\Serializable {
	<<__Native>>
	public function __construct(?string $level = NULL) : void;

	<<__Native>>
	public function getLevel() : mixed;

	<<__Native>>
	public function __debugInfo() : array;

	<<__Native>>
	function bsonSerialize() : mixed;
}

<<__NativeData("MongoDBDriverReadPreference")>>
final class ReadPreference implements \MongoDB\BSON\Serializable {
	<<__Native>>
	private function _setReadPreference(int $readPreference): void;

	<<__Native>>
	private function _setReadPreferenceTags(array $tagSets): void;

	<<__Native>>
	private function _setMaxStalenessSeconds(int $maxStalenessSeconds): void;

	public function __construct(int $readPreference, array $tagSets = null, array $options = [] )
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

				break;

			default:
				Utils::throwHippoException(Utils::ERROR_INVALID_ARGUMENT, "Invalid mode: " . $readPreference);
				break;
		}

		if ( $tagSets )
		{
			$newTagSets = [];

			foreach ( $tagSets as $tagSet )
			{
				if ( is_array( $tagSet ) )
				{
					$newTagSets[] = (object) $tagSet;
				}
				else
				{
					$newTagSets[] = $tagSet;
				}
			}

			// calling into Native, might throw exception
			$this->_setReadPreferenceTags( $newTagSets );
		}

		if ( array_key_exists( 'maxStalenessSeconds', $options ) )
		{
			$maxStalenessSeconds = (int) $options['maxStalenessSeconds'];

			if ( $maxStalenessSeconds != self::NO_MAX_STALENESS )
			{
				if ( $maxStalenessSeconds < self::SMALLEST_MAX_STALENESS_SECONDS )
				{
					Utils::throwHippoException( Utils::ERROR_INVALID_ARGUMENT, "Expected maxStalenessSeconds to be >= " . self::SMALLEST_MAX_STALENESS_SECONDS . ", {$maxStalenessSeconds} given" );
				}

				if ( $maxStalenessSeconds > 2147483647 )
				{
					Utils::throwHippoException( Utils::ERROR_INVALID_ARGUMENT, "Expected maxStalenessSeconds to be <= 2147483647, {$maxStalenessSeconds} given" );
				}
			}

			$this->_setMaxStalenessSeconds( $maxStalenessSeconds );
		}
	}

	<<__Native>>
	public function getMode() : int;

	<<__Native>>
	public function getTagSets() : array;

	<<__Native>>
	public function getMaxStalenessSeconds() : int;

	<<__Native>>
	public function __debugInfo() : array;

	<<__Native>>
	function bsonSerialize() : mixed;
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
final class WriteConcern implements \MongoDB\BSON\Serializable {
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

	<<__Native>>
	function bsonSerialize() : mixed;
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

/* {{{ APM */
namespace MongoDB\Monitoring;

class _Dispatcher
{
	static public array $subscribers = [];

	private static function _dispatchCommandStarted( \MongoDB\Driver\Monitoring\CommandStartedEvent $event )
	{
		foreach ( self::$subscribers as $subscriber )
		{
			if ( $subscriber instanceof \MongoDB\Driver\Monitoring\CommandSubscriber )
			{
				$subscriber->commandStarted( $event );
			}
		}
	}

	private static function _dispatchCommandSucceeded( \MongoDB\Driver\Monitoring\CommandSucceededEvent $event )
	{
		foreach ( self::$subscribers as $subscriber )
		{
			if ( $subscriber instanceof \MongoDB\Driver\Monitoring\CommandSubscriber )
			{
				$subscriber->commandSucceeded( $event );
			}
		}
	}

	private static function _dispatchCommandFailed( \MongoDB\Driver\Monitoring\CommandFailedEvent $event )
	{
		foreach ( self::$subscribers as $subscriber )
		{
			if ( $subscriber instanceof \MongoDB\Driver\Monitoring\CommandSubscriber )
			{
				$subscriber->commandFailed( $event );
			}
		}
	}
}

function addSubscriber( \MongoDB\Driver\Monitoring\Subscriber $subscriber ) : void
{
	/* Don't add the same subscriber twice */
	foreach ( _Dispatcher::$subscribers as $key => $item )
	{
		if ( $item === $subscriber )
		{
			return;
		}
	}

	_Dispatcher::$subscribers[] = $subscriber;
}

function removeSubscriber( \MongoDB\Driver\Monitoring\Subscriber $subscriber ) : void
{
	foreach ( _Dispatcher::$subscribers as $key => $item )
	{
		if ( $item === $subscriber )
		{
			unset( _Dispatcher::$subscribers[ $key ] );
		}
	}
}
/* }}} */

/* {{{ BSON and Serialization Classes */
namespace MongoDB\BSON;

/* {{{ Interfaces */
interface TypeWrapper
{
	static public function createFromBSONType(\MongoDB\BSON\Type $type) : \MongoDB\BSON\TypeWrapper;
	public function toBSONType();
}

interface BinaryInterface
{
	public function getType() : int;
	public function getData() : string;
	public function __toString() : string;
}

interface Decimal128Interface
{
	public function __toString() : string;
}

interface JavascriptInterface
{
	public function getCode() : string;
	public function getScope() : mixed;
	public function __toString() : string;
}

interface MaxKeyInterface
{
}

interface MinKeyInterface
{
}

interface ObjectIDInterface
{
	public function getTimestamp() : int;
	public function __toString() : string;
}

interface RegexInterface
{
	public function getPattern() : string;
	public function getFlags() : string;
	public function __toString() : string;
}

interface TimestampInterface
{
	public function __toString() : string;
}

interface UTCDateTimeInterface
{
	public function toDateTime() : \DateTime;
	public function __toString() : string;
}
/* }}} */

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

final class Binary implements Type, \Serializable, \JsonSerializable, BinaryInterface
{
	static private function checkArray(array $state)
	{
		if (
			!array_key_exists( 'data', $state ) || !is_string( $state['data'] )
			||
			!array_key_exists( 'type', $state ) || !is_int( $state['type'] )
		) {
			throw new \MongoDB\Driver\Exception\InvalidArgumentException( 'MongoDB\BSON\Binary initialization requires "data" string and "type" integer fields' );
		}
	}

	public function serialize() : string
	{
		return serialize( [
			'data' => $this->data,
			'type' => $this->type,
		] );
	}

	public function jsonSerialize() : mixed
	{
		return [
			'$binary' => base64_encode( $this->data ),
			'$type' => sprintf( "%02x", $this->type )
		];
	}

	public function unserialize(mixed $serialized) : void
	{
		$unserialized = unserialize( $serialized );
		self::checkArray( $unserialized );
		$this->__construct( $unserialized['data'], $unserialized['type']);
	}

	public function __construct(private string $data, private int $type)
	{
		if ( $type < 0 || $type > 255 )
		{
			throw new \MongoDB\Driver\Exception\InvalidArgumentException( "Expected type to be an unsigned 8-bit integer, {$type} given" );
		}
	}

	static public function __set_state(array $state)
	{
		self::checkArray( $state );
		return new self( $state['data'], $state['type'] );
	}

	public function getType() : int
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

	public function __toString() : string
	{
		return $this->data;
	}

	<<__Native>>
	public function __debugInfo() : array;
}

<<__NativeData("MongoDBBsonDecimal128")>>
final class Decimal128 implements Type, \Serializable, \JsonSerializable, Decimal128Interface
{
	static private function checkArray(array $state)
	{
		if (
			!array_key_exists( 'dec', $state ) || !is_string( $state['dec'] )
		) {
			throw new \MongoDB\Driver\Exception\InvalidArgumentException( 'MongoDB\BSON\Decimal128 initialization requires "dec" string field' );
		}
	}

	public function serialize() : string
	{
		return serialize( [
			'dec' => $this->dec,
		] );
	}

	public function jsonSerialize() : mixed
	{
		return [
			'$numberDecimal' => (string) $this->dec
		];
	}

	public function unserialize(mixed $serialized) : void
	{
		$unserialized = unserialize( $serialized );
		self::checkArray( $unserialized );
		$this->__construct( $unserialized['dec'] );
	}

	<<__Native>>
	public function __construct(string $decimal);

	static public function __set_state(array $state)
	{
		self::checkArray( $state );
		return new self( $state['dec'] );
	}

	<<__Native>>
	public function __toString() : string;

	<<__Native>>
	public function __debugInfo() : array;
}

final class Javascript implements Type, \Serializable, \JsonSerializable, JavascriptInterface
{
	private $code;
	private $scope;

	static private function checkArray(array $state)
	{
		if (
			!array_key_exists( 'code', $state ) || !is_string( $state['code'] )
		) {
			throw new \MongoDB\Driver\Exception\InvalidArgumentException( 'MongoDB\BSON\Javascript initialization requires "code" string field' );
		}

		if ( array_key_exists( 'scope', $state ) && ( $state['scope'] !== NULL ) )
		{
			if ( !is_array( $state['scope'] ) && !is_object( $state['scope'] ) )
			{
				$valueType = gettype( $state['scope'] );
				throw new \MongoDB\Driver\Exception\InvalidArgumentException( "Expected scope to be array or object, {$valueType} given" );
			}
		}
	}

	public function serialize() : string
	{
		$s = [];
		$s['code'] = $this->code;
		$s['scope'] = $this->scope ?? NULL;
		return serialize( $s );
	}

	public function jsonSerialize() : mixed
	{
		$json = [ '$code' => $this->code ];

		if ( isset( $this->scope ) )
		{
			$json['$scope'] = $this->scope;
		}

		return $json;
	}

	public function unserialize(mixed $serialized) : void
	{
		$unserialized = unserialize( $serialized );
		self::checkArray( $unserialized );
		$this->__construct( $unserialized['code'], $unserialized['scope'] ?? NULL );
	}

	public function __construct(string $code, ?mixed $scope = NULL)
	{
		if ( strstr( $code, "\0" ) !== false )
		{
			throw new \MongoDB\Driver\Exception\InvalidArgumentException( "Code cannot contain null bytes" );
		}
		if ( $scope !== NULL && ! ( is_object( $scope ) || is_array( $scope ) ) )
		{
			$valueType = gettype( $scope );
			throw new \MongoDB\Driver\Exception\InvalidArgumentException( "Expected scope to be array or object, {$valueType} given" );
		}

		$this->code = $code;
		if ( $scope !== NULL )
		{
			$this->scope = (object) $scope;
		}
	}

	static public function __set_state(array $state)
	{
		self::checkArray( $state );

		return new self( $state['code'], $state['scope'] ?? NULL );
	}

	public function __debugInfo() : array
	{
		return [
			'code' => $this->code,
			'scope' => $this->scope
		];
	}

	public function getCode() : string
	{
		return $this->code;
	}

	public function getScope() : mixed
	{
		if ( isset( $this->scope ) )
		{
			return (object) $this->scope;
		}
		return NULL;
	}

	public function __toString() : string
	{
		return $this->code;
	}
}

final class MaxKey implements Type, \Serializable, \JsonSerializable, MaxKeyInterface
{
	public function serialize() : string
	{
		return '';
	}

	public function jsonSerialize() : mixed
	{
		return [
			'$maxKey' => 1
		];
	}

	public function unserialize(mixed $serialized) : void
	{
	}

	static public function __set_state(array $state)
	{
		return new self();
	}
}

final class MinKey implements Type, \Serializable, \JsonSerializable, MinKeyInterface
{
	public function serialize() : string
	{
		return '';
	}

	public function jsonSerialize() : mixed
	{
		return [
			'$minKey' => 1
		];
	}

	public function unserialize(mixed $serialized) : void
	{
	}

	static public function __set_state(array $state)
	{
		return new self();
	}
}

<<__NativeData("MongoDBBsonObjectID")>>
final class ObjectID implements Type, \Serializable, \JsonSerializable, ObjectIDInterface
{
	static private function checkArray( array $state )
	{
		if (
			!array_key_exists( 'oid', $state ) || !is_string( $state['oid'] )
		) {
			throw new \MongoDB\Driver\Exception\InvalidArgumentException( 'MongoDB\BSON\ObjectID initialization requires "oid" string field' );
		}
	}

	public function serialize() : string
	{
		return serialize( [
			'oid' => $this->oid,
		] );
	}

	public function jsonSerialize() : mixed
	{
		return [
			'$oid' => $this->__toString()
		];
	}

	public function unserialize(mixed $serialized) : void
	{
		$unserialized = unserialize( $serialized );
		self::checkArray( $unserialized );
		$this->__construct( $unserialized['oid'] );
	}

	<<__Native>>
	public function __construct(string $objectId = null);

	static public function __set_state(array $state)
	{
		self::checkArray( $state );
		return new self( $state['oid'] );
	}

	<<__Native>>
	public function __toString() : string;

	<<__Native>>
	public function __debugInfo() : array;

	public function getTimestamp() : int
	{
		return hexdec( substr( (string) $this, 0, 8 ) );
	}
}

final class Regex implements Type, \Serializable, \JsonSerializable, RegexInterface
{
	private $pattern;
	private $flags;

	static private function checkArray( array $state )
	{
		if (
			!array_key_exists( 'pattern', $state ) || !is_string( $state['pattern'] )
			||
			!array_key_exists( 'flags', $state ) || !is_string( $state['flags'] )
		) {
			throw new \MongoDB\Driver\Exception\InvalidArgumentException( 'MongoDB\BSON\Regex initialization requires "pattern" and "flags" string fields' );
		}
	}

	public function serialize() : string
	{
		return serialize( [
			'pattern' => $this->pattern,
			'flags' => $this->flags,
		] );
	}

	public function jsonSerialize() : mixed
	{
		return [
			'$regex' => $this->pattern,
			'$options' => $this->flags,
		];
	}

	public function unserialize(mixed $serialized) : void
	{
		$unserialized = unserialize( $serialized );
		self::checkArray( $unserialized );
		$this->__construct( $unserialized['pattern'], $unserialized['flags'] );
	}

	public function __construct(string $pattern, string $flags = '')
	{
		if ( strstr( $pattern, "\0" ) !== false )
		{
			throw new \MongoDB\Driver\Exception\InvalidArgumentException( "Pattern cannot contain null bytes" );
		}
		if ( strstr( $flags, "\0" ) !== false )
		{
			throw new \MongoDB\Driver\Exception\InvalidArgumentException( "Flags cannot contain null bytes" );
		}

		/* Sort flags */
		$flags = str_split( $flags );
		sort( $flags );
		$flags = implode( '', $flags );

		$this->pattern = $pattern;
		$this->flags = $flags;
	}

	static public function __set_state(array $state)
	{
		self::checkArray( $state );
		return new self( $state['pattern'], $state['flags'] );
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

final class Timestamp implements Type, \Serializable, \JsonSerializable, TimestampInterface
{
	static private function checkArray( array $state )
	{
		if (
			( array_key_exists( 'increment', $state ) && array_key_exists( 'timestamp', $state ) )
			&&
			(
				( is_int( $state['increment'] ) && is_int( $state['timestamp'] ) )
				||
				( is_string( $state['increment'] ) && is_string( $state['timestamp'] ) )
			)
		) {
			if ( is_string( $state['increment'] ) && !is_numeric( $state['increment'] ) )
			{
				throw new \MongoDB\Driver\Exception\InvalidArgumentException( "Error parsing \"{$state['increment']}\" as 64-bit integer increment for MongoDB\BSON\Timestamp initialization" );
			}
			if ( is_string( $state['timestamp'] ) && !is_numeric( $state['timestamp'] ) )
			{
				throw new \MongoDB\Driver\Exception\InvalidArgumentException( "Error parsing \"{$state['timestamp']}\" as 64-bit integer timestamp for MongoDB\BSON\Timestamp initialization" );
			}

			return;
		}

		throw new \MongoDB\Driver\Exception\InvalidArgumentException( 'MongoDB\BSON\Timestamp initialization requires "increment" and "timestamp" integer or numeric string fields' );
	}

	public function serialize() : string
	{
		return serialize( [
			'increment' => (string) $this->increment,
			'timestamp' => (string) $this->timestamp,
		] );
	}

	public function jsonSerialize() : mixed
	{
		return [
			'$timestamp' => [
				't' => (int) $this->timestamp,
				'i' => (int) $this->increment,
			]
		];
	}

	public function unserialize(mixed $serialized) : void
	{
		$unserialized = unserialize( $serialized );
		self::checkArray( $unserialized );
		$this->__construct( (int) $unserialized['increment'], (int) $unserialized['timestamp'] );
	}

	public function __construct(mixed $increment, mixed $timestamp)
	{
		if ( !is_int( $increment ) && !is_string( $increment ) )
		{
			throw new \MongoDB\Driver\Exception\InvalidArgumentException( "Expected increment to be an unsigned 32-bit integer or string" );
		}
		if ( !is_int( $timestamp ) && !is_string( $timestamp ) )
		{
			throw new \MongoDB\Driver\Exception\InvalidArgumentException( "Expected timestamp to be an unsigned 32-bit integer or string" );
		}

		if ( $increment < 0 || $increment > 4294967295 )
		{
			throw new \MongoDB\Driver\Exception\InvalidArgumentException( "Expected increment to be an unsigned 32-bit integer, {$increment} given" );
		}
		if ( $timestamp < 0 || $timestamp > 4294967295 )
		{
			throw new \MongoDB\Driver\Exception\InvalidArgumentException( "Expected timestamp to be an unsigned 32-bit integer, {$timestamp} given" );
		}

		$this->increment = (string) $increment;
		$this->timestamp = (string) $timestamp;
	}

	static public function __set_state(array $state)
	{
		self::checkArray( $state );
		return new self( (int) $state['increment'], (int) $state['timestamp'] );
	}

	public function __toString() : string
	{
		return sprintf( "[%d:%d]", $this->increment, $this->timestamp );
	}

	public function __debugInfo() : array
	{
		return [
			'increment' => (string) $this->increment,
			'timestamp' => (string) $this->timestamp
		];
	}
}

final class UTCDateTime implements Type, \Serializable, \JsonSerializable, UTCDateTimeInterface
{
	private string $milliseconds;

	static private function checkArray( array $state )
	{
		if (
			!array_key_exists( 'milliseconds', $state ) ||
			! ( is_int( $state['milliseconds'] ) || is_string( $state['milliseconds'] ) )
		) {
			throw new \MongoDB\Driver\Exception\InvalidArgumentException( 'MongoDB\BSON\UTCDateTime initialization requires "milliseconds" integer or numeric string field' );
		}
	}

	public function serialize() : string
	{
		return serialize( [
			'milliseconds' => (string) $this->milliseconds,
		] );
	}

	public function jsonSerialize() : mixed
	{
		return [
			'$date' => [
				'$numberLong' => (string) $this->milliseconds,
			],
		];
	}

	public function unserialize(mixed $serialized) : void
	{
		$unserialized = unserialize( $serialized );
		self::checkArray( $unserialized );
		$this->__construct( $unserialized['milliseconds'] );
	}

	public function __construct(mixed $milliseconds = NULL)
	{
		if ( $milliseconds === NULL ) {
			$this->milliseconds = (string) floor( microtime( true ) * 1000 );
		} elseif ( is_object( $milliseconds ) && $milliseconds instanceof \DateTimeInterface ) {
			$this->milliseconds = (string) floor( (string) $milliseconds->format( 'U.u' ) * 1000 );
		} elseif ( is_object( $milliseconds ) ) {
			throw new \MongoDB\Driver\Exception\InvalidArgumentException( "Expected instance of DateTimeInterface, " . get_class( $milliseconds ) . " given" );
		} elseif ( ( is_string( $milliseconds ) || is_int( $milliseconds ) ) && is_numeric( $milliseconds ) ) {
			$this->milliseconds = (string) (int) $milliseconds;
		} else {
			throw new \MongoDB\Driver\Exception\InvalidArgumentException( "Error parsing \"{$milliseconds}\" as 64-bit integer for MongoDB\BSON\UTCDateTime initialization" );
		}
	}

	static public function __set_state(array $state)
	{
		self::checkArray( $state );
		return new self( $state['milliseconds'] );
	}

	public function __toString() : string
	{
		return (string) $this->milliseconds;
	}

	<<__Native>>
	public function toDateTime() : \DateTime;

	public function __debugInfo() : array
	{
		return [ 'milliseconds' => (string) $this->milliseconds ];
	}
}

/* }}} */
