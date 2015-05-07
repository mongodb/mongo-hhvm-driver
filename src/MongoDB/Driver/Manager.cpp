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

#define MONGOC_I_AM_A_DRIVER
#define delete not_delete
#include "../../../libmongoc/src/mongoc/mongoc-bulk-operation-private.h"
#undef delete
#undef MONGOC_I_AM_A_DRIVER

#include "BulkWrite.h"
#include "Command.h"
#include "Cursor.h"
#include "Manager.h"
#include "Query.h"
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
	static Class* c_result;
	bson_t *bson;
	MongoDBDriverManagerData* data = Native::data<MongoDBDriverManagerData>(this_);
	mongoc_cursor_t *cursor;
	const bson_t *doc;

	auto zquery = command->o_get(String("command"), false, s_MongoDriverCommand_className);

	VariantToBsonConverter converter(zquery, HIPPO_BSON_NO_FLAGS);
	bson = bson_new();
	converter.convert(bson);

	/* Run operation */
	cursor = mongoc_client_command(data->m_client, db.c_str(), MONGOC_QUERY_NONE, 0, 1, 0, bson, NULL, NULL);

	if (!mongoc_cursor_next(cursor, &doc)) {
		bson_error_t error;

		if (mongoc_cursor_error(cursor, &error)) {
			mongoc_cursor_destroy(cursor);
			throw MongoDriver::Utils::throwExceptionFromBsonError(&error);

			return NULL;
		}
	}

	/* Prepare result */
	c_result = Unit::lookupClass(s_MongoDriverCursor_className.get());
	assert(c_result);
	ObjectData* obj = ObjectData::newInstance(c_result);

	MongoDBDriverCursorData* cursor_data = Native::data<MongoDBDriverCursorData>(obj);

	cursor_data->cursor = cursor;
	cursor_data->hint = mongoc_cursor_get_hint(cursor);
	cursor_data->is_command_cursor = false;
	cursor_data->first_batch = doc ? bson_copy(doc) : NULL;
std::cerr << "EC cursor_data: " << cursor_data << "\n";
std::cerr << "EC first batch: " << cursor_data->first_batch << "\n";

	/* Destroy */
	bson_destroy(bson);

	return obj;
}

ObjectData *hippo_write_result_init(mongoc_write_result_t *write_result, mongoc_client_t *client, int server_id)
{
	static Class* c_writeResult;

	c_writeResult = Unit::lookupClass(s_MongoDriverWriteResult_className.get());
	assert(c_writeResult);
	ObjectData* obj = ObjectData::newInstance(c_writeResult);

	obj->o_set(String("nUpserted"), Variant((int64_t) write_result->nUpserted), s_MongoDriverWriteResult_className.get());
	obj->o_set(String("nMatched"), Variant((int64_t) write_result->nMatched), s_MongoDriverWriteResult_className.get());
	obj->o_set(String("nRemoved"), Variant((int64_t) write_result->nRemoved), s_MongoDriverWriteResult_className.get());
	obj->o_set(String("nInserted"), Variant((int64_t) write_result->nInserted), s_MongoDriverWriteResult_className.get());
	obj->o_set(String("nModified"), Variant((int64_t) write_result->nModified), s_MongoDriverWriteResult_className.get());
	obj->o_set(String("omit_nModified"), Variant((int64_t) write_result->omit_nModified), s_MongoDriverWriteResult_className.get());
	obj->o_set(String("offset"), Variant((int64_t) write_result->offset), s_MongoDriverWriteResult_className.get());
	obj->o_set(String("n_commands"), Variant((int64_t) write_result->n_commands), s_MongoDriverWriteResult_className.get());

	Variant v;

	BsonToVariantConverter convertor(bson_get_data(&write_result->upserted), write_result->upserted.len, true);
	convertor.convert(&v);
	obj->o_set(String("upsertedIds"), v, s_MongoDriverWriteResult_className);

	return obj;
}

Object HHVM_METHOD(MongoDBDriverManager, executeDelete, const String &ns, const Variant &query, const Variant &deleteOptions, const Object &writeConcern)
{
	MongoDBDriverManagerData* data = Native::data<MongoDBDriverManagerData>(this_);
	bson_t *bquery;
	auto options = deleteOptions.isNull() ? null_array : deleteOptions.toArray();
	bson_error_t error;
	bson_t reply;
	mongoc_bulk_operation_t *batch;
	char *database;
	char *collection;
	int hint;
	int flags = MONGOC_DELETE_NONE;

	VariantToBsonConverter query_converter(query, HIPPO_BSON_NO_FLAGS);
	bquery = bson_new();
	query_converter.convert(bquery);

	/* Prepare */
	if (!MongoDriver::Utils::splitNamespace(ns, &database, &collection)) {
		throw Object(SystemLib::AllocInvalidArgumentExceptionObject("Invalid namespace"));
		return NULL;
	}

	if (!options.isNull()) {
		if (options.exists(String("limit"))) {
			Variant v_multi = options[String("limit")];
			bool multi = v_multi.toBoolean();

			if (multi) {
				flags |= MONGOC_DELETE_SINGLE_REMOVE;
			}
		}
	}

	/* Run operation */
	batch = mongoc_bulk_operation_new(true);
	mongoc_bulk_operation_set_database(batch, database);
	mongoc_bulk_operation_set_collection(batch, collection);
	mongoc_bulk_operation_set_client(batch, data->m_client);

	if (flags & MONGOC_DELETE_SINGLE_REMOVE) {
		mongoc_bulk_operation_remove_one(batch, bquery);
	} else {
		mongoc_bulk_operation_remove(batch, bquery);
	}

	hint = mongoc_bulk_operation_execute(batch, &reply, &error);

	/* Prepare result */
	ObjectData* obj = hippo_write_result_init(&batch->result, data->m_client, hint);

	/* Destroy */
	bson_clear(&bquery);
	mongoc_bulk_operation_destroy(batch);

	return Object(obj);
}

Object HHVM_METHOD(MongoDBDriverManager, executeInsert, const String &ns, const Variant &document, const Object &writeConcern)
{
	bson_t *bson;
	MongoDBDriverManagerData* data = Native::data<MongoDBDriverManagerData>(this_);
	bson_error_t error;
	bson_t reply;
	mongoc_bulk_operation_t *batch;
	char *database;
	char *collection;
	int hint;

	VariantToBsonConverter converter(document, HIPPO_BSON_ADD_ID);
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

	/* Prepare result */
	ObjectData* obj = hippo_write_result_init(&batch->result, data->m_client, hint);

	/* Destroy */
	bson_destroy(bson);
	mongoc_bulk_operation_destroy(batch);

	return Object(obj);
}

Object HHVM_METHOD(MongoDBDriverManager, executeQuery, const String &ns, Object &query, Object &readPreference)
{
	static Class* c_result;
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

		VariantToBsonConverter converter(aquery[String("query")], HIPPO_BSON_NO_FLAGS);
		bson_query = bson_new();
		converter.convert(bson_query);

		if (aquery.exists(String("fields"))) {
			VariantToBsonConverter converter(aquery[String("fields")], HIPPO_BSON_NO_FLAGS);
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
			mongoc_cursor_destroy(cursor);
			throw MongoDriver::Utils::throwExceptionFromBsonError(&error);

			return NULL;
		}
	}

	/* Prepare result */
	c_result = Unit::lookupClass(s_MongoDriverCursor_className.get());
	assert(c_result);
	ObjectData* obj = ObjectData::newInstance(c_result);

	MongoDBDriverCursorData* cursor_data = Native::data<MongoDBDriverCursorData>(obj);

	cursor_data->cursor = cursor;
	cursor_data->hint = mongoc_cursor_get_hint(cursor);
	cursor_data->is_command_cursor = false;
	cursor_data->first_batch = doc ? bson_copy(doc) : NULL;
std::cerr << "EQ cursor_data: " << cursor_data << "\n";
std::cerr << "EQ first batch: " << cursor_data->first_batch << "\n";

	return obj;
}

Object HHVM_METHOD(MongoDBDriverManager, executeUpdate, const String &ns, const Variant &query, const Variant &newObj, const Variant &updateOptions, const Object &writeConcern)
{
	MongoDBDriverManagerData* data = Native::data<MongoDBDriverManagerData>(this_);
	bson_t *bquery, *bupdate;
	auto options = updateOptions.isNull() ? null_array : updateOptions.toArray();
	bson_error_t error;
	bson_t reply;
	mongoc_bulk_operation_t *batch;
	char *database;
	char *collection;
	int hint;
	int flags = MONGOC_UPDATE_NONE;

	VariantToBsonConverter query_converter(query, HIPPO_BSON_NO_FLAGS);
	bquery = bson_new();
	query_converter.convert(bquery);

	VariantToBsonConverter update_converter(newObj, HIPPO_BSON_NO_FLAGS);
	bupdate = bson_new();
	update_converter.convert(bupdate);

	/* Prepare */
	if (!MongoDriver::Utils::splitNamespace(ns, &database, &collection)) {
		throw Object(SystemLib::AllocInvalidArgumentExceptionObject("Invalid namespace"));
		return NULL;
	}

	if (!options.isNull()) {
		if (options.exists(String("multi"))) {
			Variant v_multi = options[String("multi")];
			bool multi = v_multi.toBoolean();

			if (multi) {
				flags |= MONGOC_UPDATE_MULTI_UPDATE;
			}
		}

		if (options.exists(String("upsert"))) {
			Variant v_upsert = options[String("upsert")];
			bool upsert = v_upsert.toBoolean();

			if (upsert) {
				flags |= MONGOC_UPDATE_UPSERT;
			}
		}
	}

	/* Run operation */
	batch = mongoc_bulk_operation_new(true);
	mongoc_bulk_operation_set_database(batch, database);
	mongoc_bulk_operation_set_collection(batch, collection);
	mongoc_bulk_operation_set_client(batch, data->m_client);

	if (flags & MONGOC_UPDATE_MULTI_UPDATE) {
		mongoc_bulk_operation_update(batch, bquery, bupdate, !!(flags & MONGOC_UPDATE_UPSERT));
	} else {
		bson_iter_t iter;
		bool   replaced = 0;

		if (bson_iter_init(&iter, bupdate)) {
			while (bson_iter_next(&iter)) {
				if (!strchr(bson_iter_key (&iter), '$')) {
					mongoc_bulk_operation_replace_one(batch, bquery, bupdate, !!(flags & MONGOC_UPDATE_UPSERT));
					replaced = 1;
					break;
				}
			}
		}

		if (!replaced) {
			mongoc_bulk_operation_update_one(batch, bquery, bupdate, !!(flags & MONGOC_UPDATE_UPSERT));
		}
	}

	hint = mongoc_bulk_operation_execute(batch, &reply, &error);

	/* Destroy */
	bson_clear(&bquery);
	bson_clear(&bupdate);
	mongoc_bulk_operation_destroy(batch);

	/* Prepare result */
	ObjectData* obj = hippo_write_result_init(&batch->result, data->m_client, hint);

	return Object(obj);
}

Object HHVM_METHOD(MongoDBDriverManager, executeBulkWrite, const String &ns, Object &bulk, const Object &writeConcern)
{
	MongoDBDriverManagerData* data = Native::data<MongoDBDriverManagerData>(this_);
	MongoDBDriverBulkWriteData* bulk_data = Native::data<MongoDBDriverBulkWriteData>(bulk.get());
	bson_error_t error;
	char *database;
	char *collection;
	int success;

	/* Prepare */
	if (!MongoDriver::Utils::splitNamespace(ns, &database, &collection)) {
		throw Object(SystemLib::AllocInvalidArgumentExceptionObject("Invalid namespace"));
		return NULL;
	}

	mongoc_bulk_operation_set_database(bulk_data->m_bulk, database);
	mongoc_bulk_operation_set_collection(bulk_data->m_bulk, collection);
	mongoc_bulk_operation_set_client(bulk_data->m_bulk, data->m_client);

	/* Run operation */
	success = mongoc_bulk_operation_execute(bulk_data->m_bulk, NULL, &error);

	/* Prepare result */
	if (!success) {
		/* throw exception */
		throw Object(SystemLib::AllocExceptionObject("FIXME EXCEPTION"));
	} else {
		ObjectData* obj = hippo_write_result_init(&bulk_data->m_bulk->result, data->m_client, success);

		return Object(obj);
	}
}

}
