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

#include "../../../bson.h"
#include "../../../utils.h"
#include "../../../mongodb.h"

#define MONGOC_I_AM_A_DRIVER
#include "../../../libmongoc/src/mongoc/mongoc-bulk-operation-private.h"
#include "../../../libmongoc/src/mongoc/mongoc-client-private.h"
#include "../../../libmongoc/src/mongoc/mongoc-cluster-private.h"
#undef MONGOC_I_AM_A_DRIVER

#include "BulkWrite.h"
#include "Command.h"
#include "Cursor.h"
#include "Manager.h"
#include "Query.h"
#include "ReadPreference.h"
#include "Server.h"
#include "WriteConcern.h"
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

void HHVM_METHOD(MongoDBDriverManager, __wakeup)
{
	throw MongoDriver::Utils::throwRunTimeException("Unserialization of MongoDB\\Driver\\Manager is not allowed");
}

Object HHVM_METHOD(MongoDBDriverManager, executeCommand, const String &db, const Object &command, const Variant &readPreference)
{
	bson_t *bson;
	MongoDBDriverManagerData* data = Native::data<MongoDBDriverManagerData>(this_);

	auto zquery = command->o_get(String("command"), false, s_MongoDriverCommand_className);

	VariantToBsonConverter converter(zquery, HIPPO_BSON_NO_FLAGS);
	bson = bson_new();
	converter.convert(bson);

	return MongoDriver::Utils::doExecuteCommand(
		db.c_str(),
		data->m_client,
		-1,
		bson,
		readPreference
	);
}

Object HHVM_METHOD(MongoDBDriverManager, executeDelete, const String &ns, const Variant &query, const Variant &deleteOptions, const Variant &writeConcern)
{
	MongoDBDriverManagerData* data = Native::data<MongoDBDriverManagerData>(this_);
	bson_t *bquery;
	auto options = deleteOptions.isNull() ? null_array : deleteOptions.toArray();
	bson_error_t error;
	bson_t reply;
	mongoc_bulk_operation_t *batch;
	char *database;
	char *collection;
	int server_id;
	int flags = MONGOC_DELETE_NONE;
	const mongoc_write_concern_t *write_concern = NULL;

	VariantToBsonConverter query_converter(query, HIPPO_BSON_NO_FLAGS);
	bquery = bson_new();
	query_converter.convert(bquery);

	/* Prepare */
	if (!MongoDriver::Utils::splitNamespace(ns, &database, &collection)) {
		throw MongoDriver::Utils::throwInvalidArgumentException("Invalid namespace provided: " + ns);
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

	/* Setup operation */
	batch = mongoc_bulk_operation_new(true);
	mongoc_bulk_operation_set_database(batch, database);
	mongoc_bulk_operation_set_collection(batch, collection);
	mongoc_bulk_operation_set_client(batch, data->m_client);

	if (flags & MONGOC_DELETE_SINGLE_REMOVE) {
		mongoc_bulk_operation_remove_one(batch, bquery);
	} else {
		mongoc_bulk_operation_remove(batch, bquery);
	}

	/* Deal with write concerns */
	if (!writeConcern.isNull()) {
		MongoDBDriverWriteConcernData* wc_data = Native::data<MongoDBDriverWriteConcernData>(writeConcern.toObject().get());
		write_concern = wc_data->m_write_concern;
	}
	if (write_concern) {
		mongoc_bulk_operation_set_write_concern(batch, write_concern);
	} else {
		write_concern = mongoc_client_get_write_concern(data->m_client);
	}

	/* Run operation */
	server_id = mongoc_bulk_operation_execute(batch, &reply, &error);

	/* Prepare result */
	ObjectData* obj = hippo_write_result_init(&batch->result, data->m_client, server_id, write_concern);

	/* Destroy */
	bson_clear(&bquery);
	mongoc_bulk_operation_destroy(batch);

	return Object(obj);
}

Object HHVM_METHOD(MongoDBDriverManager, executeInsert, const String &ns, const Variant &document, const Variant &writeConcern)
{
	bson_t *bson;
	MongoDBDriverManagerData* data = Native::data<MongoDBDriverManagerData>(this_);
	bson_error_t error;
	bson_t reply;
	mongoc_bulk_operation_t *batch;
	char *database;
	char *collection;
	int server_id;
	const mongoc_write_concern_t *write_concern = NULL;

	VariantToBsonConverter converter(document, HIPPO_BSON_ADD_ID);
	bson = bson_new();
	converter.convert(bson);

	/* Prepare */
	if (!MongoDriver::Utils::splitNamespace(ns, &database, &collection)) {
		throw MongoDriver::Utils::throwInvalidArgumentException("Invalid namespace provided: " + ns);
		return NULL;
	}

	batch = mongoc_bulk_operation_new(true);
	mongoc_bulk_operation_insert(batch, bson);
	mongoc_bulk_operation_set_database(batch, database);
	mongoc_bulk_operation_set_collection(batch, collection);
	mongoc_bulk_operation_set_client(batch, data->m_client);

	/* Deal with write concerns */
	if (!writeConcern.isNull()) {
		MongoDBDriverWriteConcernData* wc_data = Native::data<MongoDBDriverWriteConcernData>(writeConcern.toObject().get());
		write_concern = wc_data->m_write_concern;
	}
	if (write_concern) {
		mongoc_bulk_operation_set_write_concern(batch, write_concern);
	} else {
		write_concern = mongoc_client_get_write_concern(data->m_client);
	}

	/* Run operation */
	server_id = mongoc_bulk_operation_execute(batch, &reply, &error);

	/* Prepare result */
	ObjectData* obj = hippo_write_result_init(&batch->result, data->m_client, server_id, write_concern);

	/* Destroy */
	bson_destroy(bson);
	mongoc_bulk_operation_destroy(batch);

	return Object(obj);
}

Object HHVM_METHOD(MongoDBDriverManager, executeQuery, const String &ns, const Object &query, const Variant &readPreference)
{
	MongoDBDriverManagerData* manager_data = Native::data<MongoDBDriverManagerData>(this_);

	return MongoDriver::Utils::doExecuteQuery(
		ns,
		manager_data->m_client,
		-1,
		query,
		readPreference
	);
}

Object HHVM_METHOD(MongoDBDriverManager, executeUpdate, const String &ns, const Variant &query, const Variant &newObj, const Variant &updateOptions, const Variant &writeConcern)
{
	MongoDBDriverManagerData* data = Native::data<MongoDBDriverManagerData>(this_);
	bson_t *bquery, *bupdate;
	auto options = updateOptions.isNull() ? null_array : updateOptions.toArray();
	bson_error_t error;
	bson_t reply;
	mongoc_bulk_operation_t *batch;
	char *database;
	char *collection;
	int server_id;
	int flags = MONGOC_UPDATE_NONE;
	const mongoc_write_concern_t *write_concern = NULL;

	VariantToBsonConverter query_converter(query, HIPPO_BSON_NO_FLAGS);
	bquery = bson_new();
	query_converter.convert(bquery);

	VariantToBsonConverter update_converter(newObj, HIPPO_BSON_NO_ROOT_ODS);
	bupdate = bson_new();
	update_converter.convert(bupdate);

	/* Prepare */
	if (!MongoDriver::Utils::splitNamespace(ns, &database, &collection)) {
		throw MongoDriver::Utils::throwInvalidArgumentException("Invalid namespace provided: " + ns);
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

	/* Setup operation */
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

	/* Deal with write concerns */
	if (!writeConcern.isNull()) {
		MongoDBDriverWriteConcernData* wc_data = Native::data<MongoDBDriverWriteConcernData>(writeConcern.toObject().get());
		write_concern = wc_data->m_write_concern;
	}
	if (write_concern) {
		mongoc_bulk_operation_set_write_concern(batch, write_concern);
	} else {
		write_concern = mongoc_client_get_write_concern(data->m_client);
	}

	/* Run operation */
	server_id = mongoc_bulk_operation_execute(batch, &reply, &error);

	/* Destroy */
	bson_clear(&bquery);
	bson_clear(&bupdate);
	mongoc_bulk_operation_destroy(batch);

	/* Prepare result */
	ObjectData* obj = hippo_write_result_init(&batch->result, data->m_client, server_id, write_concern);

	return Object(obj);
}

Object HHVM_METHOD(MongoDBDriverManager, executeBulkWrite, const String &ns, const Object &bulk, const Variant &writeConcern)
{
	const mongoc_write_concern_t *write_concern = NULL;
	MongoDBDriverManagerData* manager_data = Native::data<MongoDBDriverManagerData>(this_);

	/* Deal with write concerns */
	if (!writeConcern.isNull()) {
		HPHP::MongoDBDriverWriteConcernData* wc_data = HPHP::Native::data<HPHP::MongoDBDriverWriteConcernData>(writeConcern.toObject().get());
		write_concern = wc_data->m_write_concern;
	}
	if (!write_concern) {
		write_concern = mongoc_client_get_write_concern(manager_data->m_client);
	}

	return MongoDriver::Utils::doExecuteBulkWrite(
		ns,
		manager_data->m_client,
		-1,
		bulk,
		write_concern
	);
}

Object HHVM_METHOD(MongoDBDriverManager, selectServer, const Object &readPreference)
{
	static Class* c_server;
	MongoDBDriverManagerData* data = Native::data<MongoDBDriverManagerData>(this_);
	MongoDBDriverReadPreferenceData *rp_data = Native::data<MongoDBDriverReadPreferenceData>(readPreference.get());
	uint32_t server_id;

	server_id = mongoc_cluster_preselect(&data->m_client->cluster, MONGOC_OPCODE_QUERY, rp_data->m_read_preference, NULL);

	/* Prepare result */
	c_server = Unit::lookupClass(s_MongoDriverServer_className.get());
	assert(c_server);
	ObjectData* obj = ObjectData::newInstance(c_server);

	MongoDBDriverServerData* result_data = Native::data<MongoDBDriverServerData>(obj);

	result_data->m_client = data->m_client;
	result_data->m_server_id = server_id;

	return Object(obj);
}

}
