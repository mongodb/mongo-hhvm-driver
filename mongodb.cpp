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

#include "hphp/runtime/base/base-includes.h"
#include "hphp/runtime/vm/native-data.h"
#if 0
#include "hphp/runtime/base/array-init.h"
#endif

#include "src/MongoDB/Driver/Cursor.h"
#include "src/MongoDB/Driver/CursorId.h"
#include "src/MongoDB/Driver/Manager.h"
#include "src/MongoDB/Driver/Query.h"
#include "src/MongoDB/Driver/QueryResult.h"
#include "src/MongoDB/Driver/ReadPreference.h"
#include "src/MongoDB/Driver/Server.h"
#include "src/MongoDB/Driver/WriteResult.h"

#include "mongodb.h"
#include "bson.h"
#include "utils.h"

extern "C" {
#include "libbson/src/bson/bson.h"
#include "libmongoc/src/mongoc/mongoc.h"
}

namespace HPHP {

#if 0
class Utils
{
	public:
		ObjectData* AllocInvalidArgumentException(const Variant& message);
}

ObjectData* Utils::AllocInvalidArgumentException(const Variant& message) {
	ObjectData* inst;
	TypedValue ret;

	c_invalidArgumentException = Unit::lookupClass(s_MongoDBInvalidArgumentException.get());
	inst = ObjectData::newInstance(c_invalidArgumentException);

	{
		CountableHelper cnt(inst);
		g_context->invokeFunc(
			&ret,
			->getCtor(),
			make_packed_array(message),
			inst
		);
	}
	tvRefcountedDecRef(&ret);
}
#endif

static class MongoDBExtension : public Extension {
	public:
		MongoDBExtension() : Extension("mongodb") {}

		virtual void moduleInit() {
			/* MongoDB\Driver\Manager */
			HHVM_MALIAS(MongoDB\\Driver\\Manager, __construct, MongoDBDriverManager, __construct);
			HHVM_MALIAS(MongoDB\\Driver\\Manager, executeInsert, MongoDBDriverManager, executeInsert);
			HHVM_MALIAS(MongoDB\\Driver\\Manager, executeQuery, MongoDBDriverManager, executeQuery);

			Native::registerNativeDataInfo<MongoDBDriverManagerData>(MongoDBDriverManagerData::s_className.get());

			/* MongoDb\Driver\CursorId */
			HHVM_MALIAS(MongoDB\\Driver\\CursorId, __construct, MongoDBDriverCursorId, __construct);
			HHVM_MALIAS(MongoDB\\Driver\\CursorId, __toString,  MongoDBDriverCursorId, __toString);

			Native::registerNativeDataInfo<MongoDBDriverCursorIdData>(MongoDBDriverCursorIdData::s_className.get());

			/* MongoDb\Driver\Cursor */
			HHVM_MALIAS(MongoDB\\Driver\\Cursor, getId, MongoDBDriverCursor, getId);

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

			/* MongoDb\Driver\ReadPreference */
			HHVM_MALIAS(MongoDB\\Driver\\ReadPreference, _setReadPreference, MongoDBDriverReadPreference, _setReadPreference);
			HHVM_MALIAS(MongoDB\\Driver\\ReadPreference, _setReadPreferenceTags, MongoDBDriverReadPreference, _setReadPreferenceTags);

			Native::registerNativeDataInfo<MongoDBDriverReadPreferenceData>(MongoDBDriverReadPreferenceData::s_className.get());

			Native::registerClassConstant<KindOfInt64>(s_MongoDriverReadPreference_className.get(), makeStaticString("RP_PRIMARY"), (int64_t) MONGOC_READ_PRIMARY);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverReadPreference_className.get(), makeStaticString("RP_PRIMARY_PREFERRED"), (int64_t) MONGOC_READ_PRIMARY_PREFERRED);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverReadPreference_className.get(), makeStaticString("RP_SECONDARY"), (int64_t) MONGOC_READ_SECONDARY);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverReadPreference_className.get(), makeStaticString("RP_SECONDARY_PREFERRED"), (int64_t) MONGOC_READ_SECONDARY_PREFERRED);
			Native::registerClassConstant<KindOfInt64>(s_MongoDriverReadPreference_className.get(), makeStaticString("RP_NEAREST"), (int64_t) MONGOC_READ_NEAREST);

			/* MongoDb\Driver\QueryResult */
			HHVM_MALIAS(MongoDB\\Driver\\QueryResult, getIterator, MongoDBDriverQueryResult, getIterator);
			HHVM_MALIAS(MongoDB\\Driver\\QueryResult, getServer, MongoDBDriverQueryResult, getServer);

			Native::registerNativeDataInfo<MongoDBDriverQueryResultData>(MongoDBDriverQueryResultData::s_className.get());

			/* MongoDb\Driver\Server */
			Native::registerNativeDataInfo<MongoDBDriverServerData>(MongoDBDriverServerData::s_className.get());

			loadSystemlib("mongodb");
			mongoc_init();
		}
} s_mongodb_extension;

HHVM_GET_MODULE(mongodb)

} // namespace HPHP
