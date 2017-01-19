/**
 *  Copyright 2014-2015 MongoDB, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "hphp/runtime/ext/extension.h"
#include "hphp/runtime/vm/native-data.h"
#include "hphp/util/logger.h"

#undef TRACE

#include "src/MongoDB/Driver/BulkWrite.h"
#include "src/MongoDB/Driver/Cursor.h"
#include "src/MongoDB/Driver/CursorId.h"
#include "src/MongoDB/Driver/Manager.h"
#include "src/MongoDB/Driver/Query.h"
#include "src/MongoDB/Driver/ReadConcern.h"
#include "src/MongoDB/Driver/ReadPreference.h"
#include "src/MongoDB/Driver/Server.h"
#include "src/MongoDB/Driver/WriteConcern.h"
#include "src/MongoDB/Driver/WriteResult.h"

#include "src/MongoDB/BSON/functions.h"
#include "src/MongoDB/BSON/Binary.h"
#include "src/MongoDB/BSON/Decimal128.h"
#include "src/MongoDB/BSON/Javascript.h"
#include "src/MongoDB/BSON/ObjectID.h"
#include "src/MongoDB/BSON/UTCDateTime.h"

#include "mongodb.h"
#include "bson.h"
#include "utils.h"

extern "C" {
#include "libbson/src/bson/bson.h"
#include "libmongoc/src/mongoc/mongoc.h"
#include "libmongoc/src/mongoc/mongoc-log-private.h"
#include "libmongoc/src/mongoc/mongoc-handshake.h"
}

namespace HPHP {

struct MongoDBGlobals {
	int log_method; /* 0 for off, 1 for on */
};

IMPLEMENT_THREAD_LOCAL(MongoDBGlobals, s_mongodb_globals);

void hippo_log_handler(mongoc_log_level_t log_level, const char *log_domain, const char *message, void *user_data)
{
	if (s_mongodb_globals->log_method == 0) {
		return;
	}

	switch (log_level) {
		case MONGOC_LOG_LEVEL_ERROR:
		case MONGOC_LOG_LEVEL_CRITICAL:
			Logger::Error("[HIPPO] %s %s", log_domain, message);
			break;

		case MONGOC_LOG_LEVEL_WARNING:
			Logger::Warning("[HIPPO] %s %s", log_domain, message);
			break;

		case MONGOC_LOG_LEVEL_MESSAGE:
		case MONGOC_LOG_LEVEL_INFO:
			Logger::Info("[HIPPO] %s %s", log_domain, message);
			break;

		case MONGOC_LOG_LEVEL_DEBUG:
		case MONGOC_LOG_LEVEL_TRACE:
			Logger::Verbose("[HIPPO] %s %s", log_domain, message);
			break;
	}
}

#define HIPPO_VERSION "1.2.2"

static class MongoDBExtension : public Extension {
	public:
		MongoDBExtension() : Extension("mongodb", HIPPO_VERSION) {}

		virtual void moduleInit() {
			/* MongoDB\BSON functions */
			HHVM_FALIAS(MongoDB\\BSON\\fromPHP, MongoDBBsonFromPHP);
			HHVM_FALIAS(MongoDB\\BSON\\fromJson, MongoDBBsonFromJson);
			HHVM_FALIAS(MongoDB\\BSON\\toPHP, MongoDBBsonToPHP);
			HHVM_FALIAS(MongoDB\\BSON\\toJson, MongoDBBsonToJson);

			/* MongoDB\BSON\Binary */
			Native::registerClassConstant<KindOfInt64>(s_MongoBsonBinary_className.get(), makeStaticString("TYPE_GENERIC"), (int64_t) BSON_SUBTYPE_BINARY);
			Native::registerClassConstant<KindOfInt64>(s_MongoBsonBinary_className.get(), makeStaticString("TYPE_FUNCTION"), (int64_t) BSON_SUBTYPE_FUNCTION);
			Native::registerClassConstant<KindOfInt64>(s_MongoBsonBinary_className.get(), makeStaticString("TYPE_OLD_BINARY"), (int64_t) BSON_SUBTYPE_BINARY_DEPRECATED);
			Native::registerClassConstant<KindOfInt64>(s_MongoBsonBinary_className.get(), makeStaticString("TYPE_OLD_UUID"), (int64_t) BSON_SUBTYPE_UUID_DEPRECATED);
			Native::registerClassConstant<KindOfInt64>(s_MongoBsonBinary_className.get(), makeStaticString("TYPE_UUID"), (int64_t) BSON_SUBTYPE_UUID);
			Native::registerClassConstant<KindOfInt64>(s_MongoBsonBinary_className.get(), makeStaticString("TYPE_MD5"), (int64_t) BSON_SUBTYPE_MD5);
			Native::registerClassConstant<KindOfInt64>(s_MongoBsonBinary_className.get(), makeStaticString("TYPE_USER_DEFINED"), (int64_t) BSON_SUBTYPE_USER);

			HHVM_MALIAS(MongoDB\\BSON\\Binary, __debugInfo, MongoDBBsonBinary, __debugInfo);

			/* MongoDB\BSON\Decimal128 */
			HHVM_MALIAS(MongoDB\\BSON\\Decimal128, __construct, MongoDBBsonDecimal128, __construct);
			HHVM_MALIAS(MongoDB\\BSON\\Decimal128, __debugInfo, MongoDBBsonDecimal128, __debugInfo);
			HHVM_MALIAS(MongoDB\\BSON\\Decimal128, __toString, MongoDBBsonDecimal128, __toString);

			Native::registerNativeDataInfo<MongoDBBsonDecimal128Data>(MongoDBBsonDecimal128Data::s_className.get());

			/* MongoDB\BSON\ObjectID */
			HHVM_MALIAS(MongoDB\\BSON\\ObjectID, __construct, MongoDBBsonObjectID, __construct);
			HHVM_MALIAS(MongoDB\\BSON\\ObjectID, __debugInfo, MongoDBBsonObjectID, __debugInfo);
			HHVM_MALIAS(MongoDB\\BSON\\ObjectID, __toString, MongoDBBsonObjectID, __toString);

			Native::registerNativeDataInfo<MongoDBBsonObjectIDData>(MongoDBBsonObjectIDData::s_className.get());

			/* MongoDB\BSON\UTCDateTime */
			HHVM_MALIAS(MongoDB\\BSON\\UTCDateTime, toDateTime, MongoDBBsonUTCDateTime, toDateTime);

			/* MongoDB\Driver\Manager */
			HHVM_MALIAS(MongoDB\\Driver\\Manager, __construct, MongoDBDriverManager, __construct);
			HHVM_MALIAS(MongoDB\\Driver\\Manager, __debugInfo, MongoDBDriverManager, __debugInfo);
			HHVM_MALIAS(MongoDB\\Driver\\Manager, __wakeup, MongoDBDriverManager, __wakeup);
			HHVM_MALIAS(MongoDB\\Driver\\Manager, executeBulkWrite, MongoDBDriverManager, executeBulkWrite);
			HHVM_MALIAS(MongoDB\\Driver\\Manager, executeCommand, MongoDBDriverManager, executeCommand);
			HHVM_MALIAS(MongoDB\\Driver\\Manager, executeQuery, MongoDBDriverManager, executeQuery);
			HHVM_MALIAS(MongoDB\\Driver\\Manager, getReadConcern, MongoDBDriverManager, getReadConcern);
			HHVM_MALIAS(MongoDB\\Driver\\Manager, getReadPreference, MongoDBDriverManager, getReadPreference);
			HHVM_MALIAS(MongoDB\\Driver\\Manager, getServers, MongoDBDriverManager, getServers);
			HHVM_MALIAS(MongoDB\\Driver\\Manager, getWriteConcern, MongoDBDriverManager, getWriteConcern);
			HHVM_MALIAS(MongoDB\\Driver\\Manager, selectServer, MongoDBDriverManager, selectServer);

			Native::registerNativeDataInfo<MongoDBDriverManagerData>(MongoDBDriverManagerData::s_className.get());

			/* MongoDB\Driver\BulkWrite */
			HHVM_MALIAS(MongoDB\\Driver\\BulkWrite, __construct, MongoDBDriverBulkWrite, __construct);
			HHVM_MALIAS(MongoDB\\Driver\\BulkWrite, insert, MongoDBDriverBulkWrite, insert);
			HHVM_MALIAS(MongoDB\\Driver\\BulkWrite, _update, MongoDBDriverBulkWrite, _update);
			HHVM_MALIAS(MongoDB\\Driver\\BulkWrite, _delete, MongoDBDriverBulkWrite, _delete);
			HHVM_MALIAS(MongoDB\\Driver\\BulkWrite, count, MongoDBDriverBulkWrite, count);
			HHVM_MALIAS(MongoDB\\Driver\\BulkWrite, __debugInfo, MongoDBDriverBulkWrite, __debugInfo);

			Native::registerNativeDataInfo<MongoDBDriverBulkWriteData>(MongoDBDriverBulkWriteData::s_className.get());

			/* MongoDb\Driver\CursorId */
			HHVM_MALIAS(MongoDB\\Driver\\CursorId, __debugInfo,  MongoDBDriverCursorId, __debugInfo);
			HHVM_MALIAS(MongoDB\\Driver\\CursorId, __toString,  MongoDBDriverCursorId, __toString);

			Native::registerNativeDataInfo<MongoDBDriverCursorIdData>(MongoDBDriverCursorIdData::s_className.get());

			/* MongoDb\Driver\Cursor */
			HHVM_MALIAS(MongoDB\\Driver\\Cursor, __debugInfo, MongoDBDriverCursor, __debugInfo);
			HHVM_MALIAS(MongoDB\\Driver\\Cursor, getId, MongoDBDriverCursor, getId);
			HHVM_MALIAS(MongoDB\\Driver\\Cursor, getServer, MongoDBDriverCursor, getServer);
			HHVM_MALIAS(MongoDB\\Driver\\Cursor, setTypeMap, MongoDBDriverCursor, setTypeMap);
			HHVM_MALIAS(MongoDB\\Driver\\Cursor, current, MongoDBDriverCursor, current);
			HHVM_MALIAS(MongoDB\\Driver\\Cursor, key, MongoDBDriverCursor, key);
			HHVM_MALIAS(MongoDB\\Driver\\Cursor, next, MongoDBDriverCursor, next);
			HHVM_MALIAS(MongoDB\\Driver\\Cursor, rewind, MongoDBDriverCursor, rewind);
			HHVM_MALIAS(MongoDB\\Driver\\Cursor, valid, MongoDBDriverCursor, valid);
			HHVM_MALIAS(MongoDB\\Driver\\Cursor, isDead, MongoDBDriverCursor, isDead);
			HHVM_MALIAS(MongoDB\\Driver\\Cursor, toArray, MongoDBDriverCursor, toArray);

			Native::registerNativeDataInfo<MongoDBDriverCursorData>(MongoDBDriverCursorData::s_className.get());

			/* MongoDb\Driver\Query */
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverQuery_className.get(), makeStaticString("FLAG_NONE"), (int64_t) MONGOC_QUERY_NONE);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverQuery_className.get(), makeStaticString("FLAG_TAILABLE_CURSOR"), (int64_t) MONGOC_QUERY_TAILABLE_CURSOR);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverQuery_className.get(), makeStaticString("FLAG_SLAVE_OK"), (int64_t) MONGOC_QUERY_SLAVE_OK);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverQuery_className.get(), makeStaticString("FLAG_OPLOG_REPLAY"), (int64_t) MONGOC_QUERY_OPLOG_REPLAY);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverQuery_className.get(), makeStaticString("FLAG_NO_CURSOR_TIMEOUT"), (int64_t) MONGOC_QUERY_NO_CURSOR_TIMEOUT);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverQuery_className.get(), makeStaticString("FLAG_AWAIT_DATA"), (int64_t) MONGOC_QUERY_AWAIT_DATA);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverQuery_className.get(), makeStaticString("FLAG_EXHAUST"), (int64_t) MONGOC_QUERY_EXHAUST);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverQuery_className.get(), makeStaticString("FLAG_PARTIAL"), (int64_t) MONGOC_QUERY_PARTIAL);

			/* MongoDb\Driver\ReadConcern */
			HHVM_MALIAS(MongoDB\\Driver\\ReadConcern, __construct, MongoDBDriverReadConcern, __construct);
			HHVM_MALIAS(MongoDB\\Driver\\ReadConcern, __debugInfo, MongoDBDriverReadConcern, __debugInfo);
			HHVM_MALIAS(MongoDB\\Driver\\ReadConcern, bsonSerialize, MongoDBDriverReadConcern, bsonSerialize);
			HHVM_MALIAS(MongoDB\\Driver\\ReadConcern, getLevel, MongoDBDriverReadConcern, getLevel);

			Native::registerNativeDataInfo<MongoDBDriverReadConcernData>(MongoDBDriverReadConcernData::s_className.get());

			Native::registerClassConstant<KindOfString>(s_MongoDriverReadConcern_className.get(), makeStaticString("LOCAL"), s_MongoDriverReadConcern_local.get());
			Native::registerClassConstant<KindOfString>(s_MongoDriverReadConcern_className.get(), makeStaticString("MAJORITY"), s_MongoDriverReadConcern_majority.get());
			Native::registerClassConstant<KindOfString>(s_MongoDriverReadConcern_className.get(), makeStaticString("LINEARIZABLE"), s_MongoDriverReadConcern_linearizable.get());

			/* MongoDb\Driver\ReadPreference */
			HHVM_MALIAS(MongoDB\\Driver\\ReadPreference, _setReadPreference, MongoDBDriverReadPreference, _setReadPreference);
			HHVM_MALIAS(MongoDB\\Driver\\ReadPreference, _setReadPreferenceTags, MongoDBDriverReadPreference, _setReadPreferenceTags);
			HHVM_MALIAS(MongoDB\\Driver\\ReadPreference, _setMaxStalenessSeconds, MongoDBDriverReadPreference, _setMaxStalenessSeconds);
			HHVM_MALIAS(MongoDB\\Driver\\ReadPreference, __debugInfo, MongoDBDriverReadPreference, __debugInfo);
			HHVM_MALIAS(MongoDB\\Driver\\ReadPreference, bsonSerialize, MongoDBDriverReadPreference, bsonSerialize);
			HHVM_MALIAS(MongoDB\\Driver\\ReadPreference, getTagSets, MongoDBDriverReadPreference, getTagSets);
			HHVM_MALIAS(MongoDB\\Driver\\ReadPreference, getMode, MongoDBDriverReadPreference, getMode);
			HHVM_MALIAS(MongoDB\\Driver\\ReadPreference, getMaxStalenessSeconds, MongoDBDriverReadPreference, getMaxStalenessSeconds);

			Native::registerNativeDataInfo<MongoDBDriverReadPreferenceData>(MongoDBDriverReadPreferenceData::s_className.get());

			Native::registerClassConstant<KindOfInt64>(s_MongoDriverReadPreference_className.get(), makeStaticString("RP_PRIMARY"), (int64_t) MONGOC_READ_PRIMARY);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverReadPreference_className.get(), makeStaticString("RP_PRIMARY_PREFERRED"), (int64_t) MONGOC_READ_PRIMARY_PREFERRED);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverReadPreference_className.get(), makeStaticString("RP_SECONDARY"), (int64_t) MONGOC_READ_SECONDARY);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverReadPreference_className.get(), makeStaticString("RP_SECONDARY_PREFERRED"), (int64_t) MONGOC_READ_SECONDARY_PREFERRED);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverReadPreference_className.get(), makeStaticString("RP_NEAREST"), (int64_t) MONGOC_READ_NEAREST);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverReadPreference_className.get(), makeStaticString("NO_MAX_STALENESS"), (int64_t) MONGOC_NO_MAX_STALENESS);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverReadPreference_className.get(), makeStaticString("SMALLEST_MAX_STALENESS_SECONDS"), (int64_t) MONGOC_SMALLEST_MAX_STALENESS_SECONDS);

			/* MongoDb\Driver\Server */
			HHVM_MALIAS(MongoDB\\Driver\\Server, __debugInfo, MongoDBDriverServer, __debugInfo);
			HHVM_MALIAS(MongoDB\\Driver\\Server, executeBulkWrite, MongoDBDriverServer, executeBulkWrite);
			HHVM_MALIAS(MongoDB\\Driver\\Server, executeCommand, MongoDBDriverServer, executeCommand);
			HHVM_MALIAS(MongoDB\\Driver\\Server, executeQuery, MongoDBDriverServer, executeQuery);
			HHVM_MALIAS(MongoDB\\Driver\\Server, getHost, MongoDBDriverServer, getHost);
			HHVM_MALIAS(MongoDB\\Driver\\Server, getInfo, MongoDBDriverServer, getInfo);
			HHVM_MALIAS(MongoDB\\Driver\\Server, getLatency, MongoDBDriverServer, getLatency);
			HHVM_MALIAS(MongoDB\\Driver\\Server, getPort, MongoDBDriverServer, getPort);
			HHVM_MALIAS(MongoDB\\Driver\\Server, getTags, MongoDBDriverServer, getTags);
			HHVM_MALIAS(MongoDB\\Driver\\Server, getType, MongoDBDriverServer, getType);
			HHVM_MALIAS(MongoDB\\Driver\\Server, isPrimary, MongoDBDriverServer, isPrimary);
			HHVM_MALIAS(MongoDB\\Driver\\Server, isSecondary, MongoDBDriverServer, isSecondary);
			HHVM_MALIAS(MongoDB\\Driver\\Server, isArbiter, MongoDBDriverServer, isArbiter);
			HHVM_MALIAS(MongoDB\\Driver\\Server, isHidden, MongoDBDriverServer, isHidden);
			HHVM_MALIAS(MongoDB\\Driver\\Server, isPassive, MongoDBDriverServer, isPassive);

			Native::registerNativeDataInfo<MongoDBDriverServerData>(MongoDBDriverServerData::s_className.get());

			Native::registerClassConstant<KindOfInt64>(s_MongoDriverServer_className.get(), makeStaticString("TYPE_UNKNOWN"), (int64_t) HIPPO_SERVER_UNKNOWN);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverServer_className.get(), makeStaticString("TYPE_STANDALONE"), (int64_t) HIPPO_SERVER_STANDALONE);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverServer_className.get(), makeStaticString("TYPE_MONGOS"), (int64_t) HIPPO_SERVER_MONGOS);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverServer_className.get(), makeStaticString("TYPE_POSSIBLE_PRIMARY"), (int64_t) HIPPO_SERVER_POSSIBLE_PRIMARY);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverServer_className.get(), makeStaticString("TYPE_RS_PRIMARY"), (int64_t) HIPPO_SERVER_RS_PRIMARY);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverServer_className.get(), makeStaticString("TYPE_RS_SECONDARY"), (int64_t) HIPPO_SERVER_RS_SECONDARY);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverServer_className.get(), makeStaticString("TYPE_RS_ARBITER"), (int64_t) HIPPO_SERVER_RS_ARBITER);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverServer_className.get(), makeStaticString("TYPE_RS_OTHER"), (int64_t) HIPPO_SERVER_RS_OTHER);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverServer_className.get(), makeStaticString("TYPE_RS_GHOST"), (int64_t) HIPPO_SERVER_RS_GHOST);

			/* MongoDb\Driver\WriteConcern */
			HHVM_MALIAS(MongoDB\\Driver\\WriteConcern, __construct, MongoDBDriverWriteConcern, __construct);
			HHVM_MALIAS(MongoDB\\Driver\\WriteConcern, __debugInfo, MongoDBDriverWriteConcern, __debugInfo);
			HHVM_MALIAS(MongoDB\\Driver\\WriteConcern, bsonSerialize, MongoDBDriverWriteConcern, bsonSerialize);
			HHVM_MALIAS(MongoDB\\Driver\\WriteConcern, getJournal, MongoDBDriverWriteConcern, getJournal);
			HHVM_MALIAS(MongoDB\\Driver\\WriteConcern, getW, MongoDBDriverWriteConcern, getW);
			HHVM_MALIAS(MongoDB\\Driver\\WriteConcern, getWtimeout, MongoDBDriverWriteConcern, getWtimeout);

			Native::registerNativeDataInfo<MongoDBDriverWriteConcernData>(MongoDBDriverWriteConcernData::s_className.get());

			Native::registerClassConstant<KindOfString>(s_MongoDriverWriteConcern_className.get(), makeStaticString("MAJORITY"), s_MongoDriverWriteConcern_majority.get());

			/* MongoDb\Driver\WriteResult */
			HHVM_MALIAS(MongoDB\\Driver\\WriteResult, getServer, MongoDBDriverWriteResult, getServer);
			HHVM_MALIAS(MongoDB\\Driver\\WriteResult, isAcknowledged, MongoDBDriverWriteResult, isAcknowledged);

			Native::registerNativeDataInfo<MongoDBDriverWriteResultData>(MongoDBDriverWriteResultData::s_className.get());

			loadSystemlib("mongodb");
			mongoc_init();

			std::string s = "HHVM "; s += HHVM_VERSION;
			mongoc_handshake_data_append("ext-mongodb:HHVM", HIPPO_VERSION, s.c_str());

			mongoc_log_set_handler(hippo_log_handler, NULL);
			mongoc_log_trace_enable();
		}

		void threadInit() override {
			IniSetting::Bind(
				this, IniSetting::PHP_INI_SYSTEM,
				"mongodb.debug",
				"",
				IniSetting::SetAndGet<std::string>(
					MongoDBDebugIniUpdate, MongoDBDebugIniGet
				)
			);
		}

private:
	static bool MongoDBDebugIniUpdate(const std::string& value)
	{
		if (value.empty()) {
			return false;
		}
		if (value.compare("on") == 0) {
			s_mongodb_globals->log_method = 0;
		} else {
			s_mongodb_globals->log_method = 1;
		}
		return true;
	}

	static std::string MongoDBDebugIniGet()
	{
		if (s_mongodb_globals->log_method == 1) {
			return "on";
		} else {
			return "off";
		}
	}
} s_mongodb_extension;

HHVM_GET_MODULE(mongodb)

} // namespace HPHP
