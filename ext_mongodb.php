<?hh
namespace MongoDB;

<<__NativeData("MongoDBManager")>>
class Manager {
	<<__Native>>
	function __construct(string $dsn = "localhost", array $options = array(), array $driverOptions = array()): void;

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
