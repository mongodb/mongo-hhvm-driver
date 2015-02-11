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

#include "src/MongoDB/Driver/CursorId.h"

#include "mongodb.h"
#include "bson.h"
#include "utils.h"

extern "C" {
#include "libbson/src/bson/bson.h"
#include "libmongoc/src/mongoc/mongoc.h"
#include <stdio.h>
}
namespace HPHP {

const StaticString s_MongoDriverWriteResult_className("MongoDB\\Driver\\WriteResult");

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

/* {{{ MongoDB\Driver\Cursor */
const StaticString s_MongoDriverCursor_className("MongoDB\\Driver\\Cursor");

class MongoDBDriverCursorData
{
	public:
		static Class* s_class;
		static const StaticString s_className;

		static Class* getClass();

		mongoc_cursor_t *cursor;
		int              hint;
		bool             is_command_cursor;
		bson_t          *first_batch;

		void sweep() {
		}

		~MongoDBDriverCursorData() {
			sweep();
		};
};

Class* MongoDBDriverCursorData::s_class = nullptr;
const StaticString MongoDBDriverCursorData::s_className("MongoDBDriverCursor");
IMPLEMENT_GET_CLASS(MongoDBDriverCursorData);

static Object HHVM_METHOD(MongoDBDriverCursor, getId)
{
	static Class* c_cursor;
	int64_t cursorid;
	MongoDBDriverCursorData* data = Native::data<MongoDBDriverCursorData>(this_);

	cursorid = mongoc_cursor_get_id(data->cursor);

	/* Prepare result */
	c_cursor = Unit::lookupClass(s_MongoDriverCursorId_className.get());
	assert(c_cursor);
	ObjectData* obj = ObjectData::newInstance(c_cursor);

	MongoDBDriverCursorIdData* cursorid_data = Native::data<MongoDBDriverCursorIdData>(obj);

	cursorid_data->id = cursorid;

	return Object(obj);
}
/* }}} */

/* {{{ MongoDB\Driver\Server */
const StaticString s_MongoDriverServer_className("MongoDB\\Driver\\Server");

class MongoDBDriverServerData
{
	public:
		static Class* s_class;
		static const StaticString s_className;

		static Class* getClass();

		int                 hint;
		mongoc_host_list_t *host;

		void sweep() {
		}

		~MongoDBDriverServerData() {
			sweep();
		};
};

Class* MongoDBDriverServerData::s_class = nullptr;
const StaticString MongoDBDriverServerData::s_className("MongoDBDriverServer");
IMPLEMENT_GET_CLASS(MongoDBDriverServerData);
/* }}} */

/* {{{ MongoDB\Driver\QueryResult */
const StaticString s_MongoDriverQueryResult_className("MongoDB\\Driver\\QueryResult");

class MongoDBDriverQueryResultData
{
	public:
		static Class* s_class;
		static const StaticString s_className;

		static Class* getClass();

		mongoc_cursor_t *cursor;
		int              hint;
		bool             is_command_cursor;
		bson_t          *first_batch;

		void sweep() {
		}

		~MongoDBDriverQueryResultData() {
			sweep();
		};
};

Class* MongoDBDriverQueryResultData::s_class = nullptr;
const StaticString MongoDBDriverQueryResultData::s_className("MongoDBDriverQueryResult");
IMPLEMENT_GET_CLASS(MongoDBDriverQueryResultData);

static Object HHVM_METHOD(MongoDBDriverQueryResult, getIterator)
{
	static Class* c_cursor;
	mongoc_host_list_t host;
	MongoDBDriverQueryResultData* data = Native::data<MongoDBDriverQueryResultData>(this_);

	mongoc_cursor_get_host(data->cursor, &host);

	/* Prepare result */
	c_cursor = Unit::lookupClass(s_MongoDriverCursor_className.get());
	assert(c_cursor);
	ObjectData* obj = ObjectData::newInstance(c_cursor);

	MongoDBDriverCursorData* cursor_data = Native::data<MongoDBDriverCursorData>(obj);

	cursor_data->cursor = data->cursor;
	cursor_data->hint = data->hint;
	cursor_data->is_command_cursor = data->is_command_cursor;
	cursor_data->first_batch = data->first_batch;

	return Object(obj);
}

static Object HHVM_METHOD(MongoDBDriverQueryResult, getServer)
{
	static Class* c_server;
	mongoc_host_list_t host;
	MongoDBDriverQueryResultData* data = Native::data<MongoDBDriverQueryResultData>(this_);

	mongoc_cursor_get_host(data->cursor, &host);

	/* Prepare result */
	c_server = Unit::lookupClass(s_MongoDriverServer_className.get());
	assert(c_server);
	ObjectData* obj = ObjectData::newInstance(c_server);

	MongoDBDriverServerData* result_data = Native::data<MongoDBDriverServerData>(obj);

	result_data->hint = data->hint;
	result_data->host = &host;

	return Object(obj);
}

/* }}} */

/* {{{ MongoDB\Driver\Query */
const StaticString s_MongoDriverQuery_className("MongoDB\\Driver\\Query");
/* }}} */

/* {{{ MongoDB\Driver\Manager */
class MongoDBDriverManagerData
{
	public:
		static Class* s_class;
		static const StaticString s_className;

		mongoc_client_t *m_client;

		static Class* getClass();

		void sweep() {
			mongoc_client_destroy(m_client);
		}

		~MongoDBDriverManagerData() {
			sweep();
		};
};

Class* MongoDBDriverManagerData::s_class = nullptr;
const StaticString MongoDBDriverManagerData::s_className("MongoDBDriverManager");
IMPLEMENT_GET_CLASS(MongoDBDriverManagerData);

static void HHVM_METHOD(MongoDBDriverManager, __construct, const String &dsn, const Array &options, const Array &driverOptions)
{
	MongoDBDriverManagerData* data = Native::data<MongoDBDriverManagerData>(this_);
	mongoc_client_t *client;

	client = mongoc_client_new(dsn.c_str());

	if (!client) {
		throw Object(SystemLib::AllocExceptionObject("Can't connect"));
	}

	data->m_client = client;
}

static Object HHVM_METHOD(MongoDBDriverManager, executeInsert, const String &ns, const Variant &document, const Object &writeConcern)
{
	static Class* c_writeResult;
	bson_t *bson;
	MongoDBDriverManagerData* data = Native::data<MongoDBDriverManagerData>(this_);
	bson_error_t error;
	bson_t reply;
	mongoc_bulk_operation_t *batch;
	char *database;
	char *collection;
	int hint;

	VariantToBsonConverter converter(document);
	bson = bson_new();
	converter.convert(bson);

	/* Prepare */
	if (!MongoDriver::Utils::splitNamespace(ns, &database, &collection)) {
		throw Object(SystemLib::AllocInvalidArgumentExceptionObject("Invalid namespace"));
		return NULL;
	}

	batch = mongoc_bulk_operation_new(true);
	mongoc_bulk_operation_insert(batch, bson);
	mongoc_bulk_operation_set_database(batch, database);
	mongoc_bulk_operation_set_collection(batch, collection);
	mongoc_bulk_operation_set_client(batch, data->m_client);

	/* Run operation */
	hint = mongoc_bulk_operation_execute(batch, &reply, &error);

	/* Destroy */
	bson_destroy(bson);
	mongoc_bulk_operation_destroy(batch);

	/* Prepare result */

	c_writeResult = Unit::lookupClass(s_MongoDriverWriteResult_className.get());
	assert(c_writeResult);
	ObjectData* obj = ObjectData::newInstance(c_writeResult);

	obj->o_set(String("nInserted"), Variant(52), s_MongoDriverWriteResult_className.get());
	obj->o_set(String("nModified"), Variant(77), s_MongoDriverWriteResult_className.get());

	return Object(obj);
}

static Object HHVM_METHOD(MongoDBDriverManager, executeQuery, const String &ns, Object &query, Object &readPreference)
{
	static Class* c_queryResult;
	bson_t *bson_query = NULL, *bson_fields = NULL;
	const bson_t *doc;
	MongoDBDriverManagerData* manager_data = Native::data<MongoDBDriverManagerData>(this_);
	char *dbname;
	char *collname;
	mongoc_collection_t *collection;
	mongoc_cursor_t *cursor;

	uint32_t skip, limit, batch_size;
	mongoc_query_flags_t flags;

	/* Prepare */
	if (!MongoDriver::Utils::splitNamespace(ns, &dbname, &collname)) {
		throw Object(SystemLib::AllocInvalidArgumentExceptionObject("Invalid namespace"));
		return NULL;
	}

	/* Get query properties */
	auto zquery = query->o_get(String("query"), false, s_MongoDriverQuery_className);

	if (zquery.getType() == KindOfArray) {
		const Array& aquery = zquery.toArray();

		skip = aquery[String("skip")].toInt32();
		limit = aquery[String("limit")].toInt32();
		batch_size = aquery[String("batch_size")].toInt32();
		flags = (mongoc_query_flags_t) aquery[String("flags")].toInt32();

		VariantToBsonConverter converter(aquery[String("query")]);
		bson_query = bson_new();
		converter.convert(bson_query);

		if (aquery.exists(String("fields"))) {
			VariantToBsonConverter converter(aquery[String("fields")]);
			bson_fields = bson_new();
			converter.convert(bson_fields);
		}
	}

	/* Run query and get cursor */
	collection = mongoc_client_get_collection(manager_data->m_client, dbname, collname);
	cursor = mongoc_collection_find(collection, flags, skip, limit, batch_size, bson_query, bson_fields, NULL /*read_preference*/);
	mongoc_collection_destroy(collection);

	/* Check for errors */
	if (!mongoc_cursor_next(cursor, &doc)) {
		bson_error_t error;

		/* Could simply be no docs, which is not an error */
		if (mongoc_cursor_error(cursor, &error)) {
//			MongoDriver::Utils::throwExceptionFromBsonError(&error);
			mongoc_cursor_destroy(cursor);
			return NULL;
		}
	}

	/* Prepare result */
	c_queryResult = Unit::lookupClass(s_MongoDriverQueryResult_className.get());
	assert(c_queryResult);
	ObjectData* obj = ObjectData::newInstance(c_queryResult);

	MongoDBDriverQueryResultData* result_data = Native::data<MongoDBDriverQueryResultData>(obj);

	result_data->cursor = cursor;
	result_data->hint = mongoc_cursor_get_hint(cursor);
	result_data->is_command_cursor = false;
	result_data->first_batch = doc ? bson_copy(doc) : NULL;

	return Object(obj);
}
/* }}} */

/* {{{ MongoDB\Driver\ReadPreference */
const StaticString s_MongoDriverReadPreference_className("MongoDB\\Driver\\ReadPreference");

class MongoDBDriverReadPreferenceData
{
	public:
		static Class* s_class;
		static const StaticString s_className;

		mongoc_read_prefs_t *m_read_preference = NULL;

		static Class* getClass();

		void sweep() {
			mongoc_read_prefs_destroy(m_read_preference);
		}

		~MongoDBDriverReadPreferenceData() {
			sweep();
		};
};

Class* MongoDBDriverReadPreferenceData::s_class = nullptr;
const StaticString MongoDBDriverReadPreferenceData::s_className("MongoDBDriverReadPreference");
IMPLEMENT_GET_CLASS(MongoDBDriverReadPreferenceData);

static void HHVM_METHOD(MongoDBDriverReadPreference, _setReadPreference, int readPreference)
{
	MongoDBDriverReadPreferenceData* data = Native::data<MongoDBDriverReadPreferenceData>(this_);

	data->m_read_preference = mongoc_read_prefs_new((mongoc_read_mode_t) readPreference);
}

static void HHVM_METHOD(MongoDBDriverReadPreference, _setReadPreferenceTags, const Variant &tagSets)
{
	MongoDBDriverReadPreferenceData* data = Native::data<MongoDBDriverReadPreferenceData>(this_);
	bson_t *bson;

	/* Convert argument */
	VariantToBsonConverter converter(tagSets);
	bson = bson_new();
	converter.convert(bson);

	/* Set and check errors */
	mongoc_read_prefs_set_tags(data->m_read_preference, bson);
	bson_destroy(bson);
	if (!mongoc_read_prefs_is_valid(data->m_read_preference)) {
		/* Throw exception */
		throw Object(SystemLib::AllocInvalidArgumentExceptionObject("Invalid tagSet"));
	}
}
/* }}} */

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
