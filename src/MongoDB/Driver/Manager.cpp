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

#include "../../../bson.h"
#include "../../../utils.h"
#include "../../../mongodb.h"

#include "Command.h"
#include "Manager.h"
#include "Query.h"
#include "QueryResult.h"
#include "WriteResult.h"

namespace HPHP {

Class* MongoDBDriverManagerData::s_class = nullptr;
const StaticString MongoDBDriverManagerData::s_className("MongoDBDriverManager");
IMPLEMENT_GET_CLASS(MongoDBDriverManagerData);

void HHVM_METHOD(MongoDBDriverManager, __construct, const String &dsn, const Array &options, const Array &driverOptions)
{
	MongoDBDriverManagerData* data = Native::data<MongoDBDriverManagerData>(this_);
	mongoc_client_t *client;

	client = mongoc_client_new(dsn.c_str());

	if (!client) {
		throw Object(SystemLib::AllocExceptionObject("Can't connect"));
	}

	data->m_client = client;
}

Object HHVM_METHOD(MongoDBDriverManager, executeCommand, const String &db, Object &command, Object &readPreference)
{
	static Class* c_queryResult;
	bson_t *bson;
	MongoDBDriverManagerData* data = Native::data<MongoDBDriverManagerData>(this_);
	mongoc_cursor_t *cursor;
	const bson_t *doc;

	auto zquery = command->o_get(String("command"), false, s_MongoDriverCommand_className);

	VariantToBsonConverter converter(zquery);
	bson = bson_new();
	converter.convert(bson);

	/* Run operation */
	cursor = mongoc_client_command(data->m_client, db.c_str(), MONGOC_QUERY_NONE, 0, 1, 0, bson, NULL, NULL);

	if (!mongoc_cursor_next(cursor, &doc)) {
		bson_error_t error;

		if (mongoc_cursor_error(cursor, &error)) {
			mongoc_cursor_destroy(cursor);
//			MongoDriver::Utils::throwExceptionFromBsonError(&error);
			return NULL;
		}
	}

	/* Destroy */
	bson_destroy(bson);

	/* Prepare result */
	c_queryResult = Unit::lookupClass(s_MongoDriverQueryResult_className.get());
	assert(c_queryResult);
	ObjectData* obj = ObjectData::newInstance(c_queryResult);

	return Object(obj);
}

Object HHVM_METHOD(MongoDBDriverManager, executeInsert, const String &ns, const Variant &document, const Object &writeConcern)
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

Object HHVM_METHOD(MongoDBDriverManager, executeQuery, const String &ns, Object &query, Object &readPreference)
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

}
