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

#include "bson.h"
#include "utils.h"

extern "C" {
#include "libbson/src/bson/bson.h"
#include "libmongoc/src/mongoc/mongoc.h"
#include <stdio.h>
}

#define IMPLEMENT_GET_CLASS(cls) \
	Class* cls::getClass() { \
		if (s_class == nullptr) { \
			s_class = Unit::lookupClass(s_className.get()); \
			assert(s_class); \
		} \
		return s_class; \
	}

namespace HPHP {

const StaticString s_MongoDriverWriteResult_className("MongoDB\\Driver\\WriteResult");

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
/* }}} */

/* {{{ MongoDB\Manager */
class MongoDBManagerData
{
	public:
		static Class* s_class;
		static const StaticString s_className;

		mongoc_client_t *m_client;

		static Class* getClass();

		void sweep() {
			mongoc_client_destroy(m_client);
		}

		~MongoDBManagerData() {
			sweep();
		};
};

Class* MongoDBManagerData::s_class = nullptr;
const StaticString MongoDBManagerData::s_className("MongoDBManager");
IMPLEMENT_GET_CLASS(MongoDBManagerData);

static void HHVM_METHOD(MongoDBManager, __construct, const String &dsn, const Array &options, const Array &driverOptions)
{
	MongoDBManagerData* data = Native::data<MongoDBManagerData>(this_);
	mongoc_client_t *client;

	client = mongoc_client_new(dsn.c_str());

	if (!client) {
		throw Object(SystemLib::AllocExceptionObject("Can't connect"));
	}

	data->m_client = client;
}

static Object HHVM_METHOD(MongoDBManager, executeInsert, const String &ns, const Variant &document, const Object &writeConcern)
{
	static Class* c_foobar;
	bson_t *bson;
	MongoDBManagerData* data = Native::data<MongoDBManagerData>(this_);
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
	MongoDriver::Utils::splitNamespace(ns, &database, &collection);

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

	c_foobar = Unit::lookupClass(s_MongoDriverWriteResult_className.get());
	assert(c_foobar);
	ObjectData* obj = ObjectData::newInstance(c_foobar);

	obj->o_set(String("nInserted"), Variant(52), s_MongoDriverWriteResult_className.get());
	obj->o_set(String("nModified"), Variant(77), s_MongoDriverWriteResult_className.get());

	return Object(obj);
}
/* }}} */

/* {{{ MongoDB\Driver\Query */
const StaticString s_MongoDriverQuery_className("MongoDB\\Driver\\Query");
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
			/* MongoDB\Manager */
			HHVM_MALIAS(MongoDB\\Manager, __construct, MongoDBManager, __construct);
			HHVM_MALIAS(MongoDB\\Manager, executeInsert, MongoDBManager, executeInsert);

			Native::registerNativeDataInfo<MongoDBManagerData>(MongoDBManagerData::s_className.get());

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
			Native::registerNativeDataInfo<MongoDBDriverQueryResultData>(MongoDBDriverQueryResultData::s_className.get());

			loadSystemlib("mongodb");
			mongoc_init();
		}
} s_mongodb_extension;

HHVM_GET_MODULE(mongodb)

} // namespace HPHP
