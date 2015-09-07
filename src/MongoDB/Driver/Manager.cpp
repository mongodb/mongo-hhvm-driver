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
#include "hphp/runtime/base/array-iterator.h"

#include "../../../bson.h"
#include "../../../utils.h"
#include "../../../mongodb.h"

#include "../../../libmongoc/src/mongoc/mongoc-client.h"
#define MONGOC_I_AM_A_DRIVER
#include "../../../libmongoc/src/mongoc/mongoc-bulk-operation-private.h"
#include "../../../libmongoc/src/mongoc/mongoc-client-private.h"
#include "../../../libmongoc/src/mongoc/mongoc-cluster-private.h"
#include "../../../libmongoc/src/mongoc/mongoc-write-concern-private.h"
#include "../../../libmongoc/src/mongoc/mongoc-uri-private.h"
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

const StaticString
	s_MongoDBDriverManager_slaveok("slaveok"),
	s_MongoDBDriverManager_readpreference("readpreference"),
	s_MongoDBDriverManager_readpreferencetags("readpreferencetags"),
	s_MongoDBDriverManager_readPreference("readPreference"),
	s_MongoDBDriverManager_readPreferenceTags("readPreferenceTags"),
	s_MongoDBDriverManager_mode("mode"),
	s_MongoDBDriverManager_tags("tags"),
	s_MongoDBDriverManager_w("w"),
	s_MongoDBDriverManager_wmajority("wmajority"),
	s_MongoDBDriverManager_wtimeout("wtimeout"),
	s_MongoDBDriverManager_wtimeoutms("wtimeoutms"),
	s_MongoDBDriverManager_fsync("fsync"),
	s_MongoDBDriverManager_safe("safe"),
	s_MongoDBDriverManager_journal("journal");

static bool hippo_mongo_driver_manager_apply_rp(mongoc_client_t *client, const Array options)
{
	mongoc_read_prefs_t *new_rp;
	const mongoc_read_prefs_t *old_rp;
	const char *rp_str = NULL;
	bson_t *b_tags;

	if (!(old_rp = mongoc_client_get_read_prefs(client))) {
		throw MongoDriver::Utils::throwRunTimeException("Client does not have a read preference");

		return false;
	}

	if (options.size() == 0) {
		return true;
	}

	if (
		!options.exists(s_MongoDBDriverManager_slaveok) &&
		!options.exists(s_MongoDBDriverManager_readpreference) &&
		!options.exists(s_MongoDBDriverManager_readpreferencetags) &&
		!options.exists(s_MongoDBDriverManager_readPreference) &&
		!options.exists(s_MongoDBDriverManager_readPreferenceTags)
	) {
		return true;
	}

	new_rp = mongoc_read_prefs_copy(old_rp);

	if (options.exists(s_MongoDBDriverManager_slaveok) && options[s_MongoDBDriverManager_slaveok].isBoolean()) {
		mongoc_read_prefs_set_mode(new_rp, MONGOC_READ_SECONDARY_PREFERRED);
	}

	if (options.exists(s_MongoDBDriverManager_readpreference) && options[s_MongoDBDriverManager_readpreference].isString()) {
		rp_str = options[s_MongoDBDriverManager_readpreference].toString().c_str();
	}
	if (options.exists(s_MongoDBDriverManager_readPreference) && options[s_MongoDBDriverManager_readPreference].isString()) {
		rp_str = options[s_MongoDBDriverManager_readPreference].toString().c_str();
	}

	if (rp_str) {
		if (0 == strcasecmp("primary", rp_str)) {
			mongoc_read_prefs_set_mode(new_rp, MONGOC_READ_PRIMARY);
		} else if (0 == strcasecmp("primarypreferred", rp_str)) {
			mongoc_read_prefs_set_mode(new_rp, MONGOC_READ_PRIMARY_PREFERRED);
		} else if (0 == strcasecmp("secondary", rp_str)) {
			mongoc_read_prefs_set_mode(new_rp, MONGOC_READ_SECONDARY);
		} else if (0 == strcasecmp("secondarypreferred", rp_str)) {
			mongoc_read_prefs_set_mode(new_rp, MONGOC_READ_SECONDARY_PREFERRED);
		} else if (0 == strcasecmp("nearest", rp_str)) {
			mongoc_read_prefs_set_mode(new_rp, MONGOC_READ_NEAREST);
		} else {
			throw MongoDriver::Utils::throwInvalidArgumentException("Unsupported readPreference value: '" + Variant(rp_str).toString() + "'");
			mongoc_read_prefs_destroy(new_rp);

			return false;
		}
	}

	if (options.exists(s_MongoDBDriverManager_readpreferencetags) && options[s_MongoDBDriverManager_readpreferencetags].isArray()) {
		VariantToBsonConverter converter(options[s_MongoDBDriverManager_readpreferencetags].toArray(), HIPPO_BSON_NO_FLAGS);
		b_tags = bson_new();
		converter.convert(b_tags);
		mongoc_read_prefs_set_tags(new_rp, b_tags);
	} else if (options.exists(s_MongoDBDriverManager_readPreferenceTags) && options[s_MongoDBDriverManager_readPreferenceTags].isArray()) {
		VariantToBsonConverter converter(options[s_MongoDBDriverManager_readPreferenceTags].toArray(), HIPPO_BSON_NO_FLAGS);
		b_tags = bson_new();
		converter.convert(b_tags);
		mongoc_read_prefs_set_tags(new_rp, b_tags);
	}

	if (
		mongoc_read_prefs_get_mode(new_rp) == MONGOC_READ_PRIMARY &&
		!bson_empty(mongoc_read_prefs_get_tags(new_rp))
	) {
		throw MongoDriver::Utils::throwInvalidArgumentException("Primary read preference mode conflicts with tags");
		mongoc_read_prefs_destroy(new_rp);

		return false;
	}

	/* This may be redundant in light of the last check (primary with tags),
	 * but we'll check anyway in case additional validation is implemented. */
	if (!mongoc_read_prefs_is_valid(new_rp)) {
		throw MongoDriver::Utils::throwInvalidArgumentException("Read preference is not valid");
		mongoc_read_prefs_destroy(new_rp);

		return false;
	}

	mongoc_client_set_read_prefs(client, new_rp);
	mongoc_read_prefs_destroy(new_rp);

	return true;
}

static bool hippo_mongo_driver_manager_apply_wc(mongoc_client_t *client, const Array options)
{
	int32_t wtimeoutms;
	mongoc_write_concern_t *new_wc;
	const mongoc_write_concern_t *old_wc;

	if (!(old_wc = mongoc_client_get_write_concern(client))) {
		throw MongoDriver::Utils::throwRunTimeException("Client does not have a write concern");

		return false;
	}

	if (options.size() == 0) {
		return true;
	}

	if (
		!options.exists(s_MongoDBDriverManager_journal) &&
		!options.exists(s_MongoDBDriverManager_safe) &&
		!options.exists(s_MongoDBDriverManager_w) &&
		!options.exists(s_MongoDBDriverManager_wtimeoutms)
	) {
		return true;
	}

	wtimeoutms = mongoc_write_concern_get_wtimeout(old_wc);

	new_wc = mongoc_write_concern_copy(old_wc);

	if (options.exists(s_MongoDBDriverManager_safe) && options[s_MongoDBDriverManager_safe].isBoolean()) {
		mongoc_write_concern_set_w(new_wc, options[s_MongoDBDriverManager_safe].toBoolean() ? 1 : MONGOC_WRITE_CONCERN_W_UNACKNOWLEDGED);
	}

	if (options.exists(s_MongoDBDriverManager_wtimeoutms) && options[s_MongoDBDriverManager_wtimeoutms].isInteger()) {
		wtimeoutms = (int32_t) options[s_MongoDBDriverManager_wtimeoutms].toInt64();
	}

	if (options.exists(s_MongoDBDriverManager_journal) && options[s_MongoDBDriverManager_journal].isBoolean()) {
		mongoc_write_concern_set_journal(new_wc, !!options[s_MongoDBDriverManager_journal].toBoolean());
	}

	if (options.exists(s_MongoDBDriverManager_w)) {
		if (options[s_MongoDBDriverManager_w].isInteger()) {
			int32_t value = (int32_t) options[s_MongoDBDriverManager_w].toInt64();

			switch (value) {
				case MONGOC_WRITE_CONCERN_W_ERRORS_IGNORED:
				case MONGOC_WRITE_CONCERN_W_UNACKNOWLEDGED:
					mongoc_write_concern_set_w(new_wc, value);
					break;

				default:
					if (value > 0) {
						mongoc_write_concern_set_w(new_wc, value);
						break;
					}
					throw MongoDriver::Utils::throwInvalidArgumentException("Unsupported w value: " + Variant(value).toString());
					mongoc_write_concern_destroy(new_wc);

					return false;
			}
		} else if (options[s_MongoDBDriverManager_w].isString()) {
			const char *str = options[s_MongoDBDriverManager_w].toString().c_str();

			if (0 == strcasecmp("majority", str)) {
				mongoc_write_concern_set_wmajority(new_wc, wtimeoutms);
			} else {
				mongoc_write_concern_set_wtag(new_wc, str);
			}
		}
	}

	/* Only set wtimeout if it's still applicable; otherwise, clear it. */
	if (mongoc_write_concern_get_w(new_wc) > 1 ||
		mongoc_write_concern_get_wmajority(new_wc) ||
		mongoc_write_concern_get_wtag(new_wc)) {
		mongoc_write_concern_set_wtimeout(new_wc, wtimeoutms);
	} else {
		mongoc_write_concern_set_wtimeout(new_wc, 0);
	}

	if (mongoc_write_concern_get_journal(new_wc)) {
		int32_t w = mongoc_write_concern_get_w(new_wc);

		if (w == MONGOC_WRITE_CONCERN_W_UNACKNOWLEDGED || w == MONGOC_WRITE_CONCERN_W_ERRORS_IGNORED) {
			throw MongoDriver::Utils::throwInvalidArgumentException("Journal conflicts with w value: " + Variant(w).toString());
			mongoc_write_concern_destroy(new_wc);

			return false;
		}
	}

	/* This may be redundant in light of the last check (unacknowledged w with
	 * journal), but we'll check anyway in case additional validation is
	 * implemented. */
	if (!_mongoc_write_concern_is_valid(new_wc)) {
		throw MongoDriver::Utils::throwInvalidArgumentException("Write concern is not valid");
		mongoc_write_concern_destroy(new_wc);

		return false;
	}

	mongoc_client_set_write_concern(client, new_wc);
	mongoc_write_concern_destroy(new_wc);

	return true;
}

static mongoc_uri_t *hippo_mongo_driver_manager_make_uri(const char *dsn, const Array options)
{
	mongoc_uri_t *uri = mongoc_uri_new(dsn);

	if (!uri) {
		throw MongoDriver::Utils::throwInvalidArgumentException("Failed to parse MongoDB URI: '" + String(dsn) + "'");
	}

	for (ArrayIter iter(options); iter; ++iter) {
		const Variant& key = iter.first();
		const Variant& value = iter.second();
		const char *s_key = key.toString().c_str();

		if (
			!strcasecmp(s_key, "journal") ||
			!strcasecmp(s_key, "readpreference") ||
			!strcasecmp(s_key, "readpreferencetags") ||
			!strcasecmp(s_key, "safe") ||
			!strcasecmp(s_key, "slaveok") ||
			!strcasecmp(s_key, "w") ||
			!strcasecmp(s_key, "wtimeoutms")
		) {
			continue;
		}

		if (mongoc_uri_option_is_bool(s_key)) {
			mongoc_uri_set_option_as_bool(uri, s_key, value.toBoolean());
		} else if (mongoc_uri_option_is_int32(s_key) && value.isInteger()) {
			mongoc_uri_set_option_as_int32(uri, s_key, (int32_t) value.toInt64());
		} else if (mongoc_uri_option_is_utf8(s_key) && value.isString()) {
			mongoc_uri_set_option_as_utf8(uri, s_key, value.toString().c_str());
		} else if (value.isArray()) {
			for (ArrayIter h_iter(value.toArray()); h_iter; ++h_iter) {
				const Variant& h_value = h_iter.second();

				if (h_value.isString()) {
					mongoc_uri_parse_host(uri, h_value.toString().c_str());
				}
			}
		} else if (value.isString()) {
			if (!strcasecmp(s_key, "username")) {
				mongoc_uri_set_username(uri, value.toString().c_str());
			} else if (!strcasecmp(s_key, "password")) {
				mongoc_uri_set_password(uri, value.toString().c_str());
			} else if (!strcasecmp(s_key, "database")) {
				mongoc_uri_set_database(uri, value.toString().c_str());
			} else if (!strcasecmp(s_key, "authsource")) {
				mongoc_uri_set_auth_source(uri, value.toString().c_str());
			}
		}
	}

	return uri;
}

void HHVM_METHOD(MongoDBDriverManager, __construct, const String &dsn, const Array &options, const Array &driverOptions)
{
	MongoDBDriverManagerData* data = Native::data<MongoDBDriverManagerData>(this_);
	mongoc_uri_t *uri;
	mongoc_client_t *client;

	uri = hippo_mongo_driver_manager_make_uri(dsn.c_str(), options);
	client = mongoc_client_new_from_uri(uri);

	if (!client) {
		throw MongoDriver::Utils::throwRunTimeException("Failed to create Manager from URI: '" + dsn + "'");
	}

	data->m_client = client;

	hippo_mongo_driver_manager_apply_rp(data->m_client, options);
	hippo_mongo_driver_manager_apply_wc(data->m_client, options);
}

const StaticString
	s_MongoDBDriverManager_request_id("request_id"),
	s_MongoDBDriverManager_uri("uri"),
	s_MongoDBDriverManager_cluster("cluster");

Array HHVM_METHOD(MongoDBDriverManager, __debugInfo)
{
	MongoDBDriverManagerData* data = Native::data<MongoDBDriverManagerData>(this_);
	Array retval = Array::Create();
	Array servers = Array::Create();

	retval.add(s_MongoDBDriverManager_request_id, (int64_t) data->m_client->request_id);
	retval.add(s_MongoDBDriverManager_uri, (char*) mongoc_uri_get_string(data->m_client->uri));

	mongoc_set_for_each(data->m_client->topology->description.servers, mongodb_driver_add_server_debug_wrapper, &servers);

	retval.add(s_MongoDBDriverManager_cluster, servers);

	return retval;
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
	ObjectData* obj = hippo_write_result_init(&batch->result, data->m_client, server_id, write_concern, true);

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
	ObjectData* obj = hippo_write_result_init(&batch->result, data->m_client, server_id, write_concern, true);

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
	ObjectData* obj = hippo_write_result_init(&batch->result, data->m_client, server_id, write_concern, true);

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

Array HHVM_METHOD(MongoDBDriverManager, getReadPreference)
{
	MongoDBDriverManagerData* data = Native::data<MongoDBDriverManagerData>(this_);
	const mongoc_read_prefs_t *read_prefs = mongoc_client_get_read_prefs(data->m_client);

	Array retval = Array::Create();

	retval.add(s_MongoDBDriverManager_mode, read_prefs->mode);
	if (read_prefs->tags.len) {
		Variant v_tags;

		hippo_bson_conversion_options_t options = HIPPO_TYPEMAP_DEBUG_INITIALIZER;
		BsonToVariantConverter convertor(bson_get_data(&read_prefs->tags), read_prefs->tags.len, options);
		convertor.convert(&v_tags);

		retval.add(s_MongoDBDriverManager_tags, v_tags);
	} else {
		retval.add(s_MongoDBDriverManager_tags, Variant());
	}

	return retval;
}

Array HHVM_METHOD(MongoDBDriverManager, getServers)
{
	MongoDBDriverManagerData *data = Native::data<MongoDBDriverManagerData>(this_);
	size_t                    i;
	mongoc_set_t             *set;

	Array retval = Array::Create();

	set = data->m_client->topology->description.servers;
	for (i = 0; i < set->items_len; i++) {
		mongoc_server_description_t *sd = (mongoc_server_description_t*) set->items[i].item;

		if (sd->type == MONGOC_SERVER_UNKNOWN) {
			continue;
		}

		retval.add(
			(int64_t) i,
			hippo_mongo_driver_server_create_from_id(data->m_client, sd->id)
		);

	}

	return retval;
}

Array HHVM_METHOD(MongoDBDriverManager, getWriteConcern)
{
	MongoDBDriverManagerData* data = Native::data<MongoDBDriverManagerData>(this_);
	const mongoc_write_concern_t *write_concern = mongoc_client_get_write_concern(data->m_client);
	const char *wtag = mongoc_write_concern_get_wtag(write_concern);
	const int32_t w = mongoc_write_concern_get_w(write_concern);

	Array retval = Array::Create();

	if (wtag) {
		retval.add(s_MongoDBDriverManager_w, wtag);
	} else if (mongoc_write_concern_get_wmajority(write_concern)) {
		retval.add(s_MongoDBDriverManager_w, "majority");
	} else if (w != MONGOC_WRITE_CONCERN_W_DEFAULT) {
		retval.add(s_MongoDBDriverManager_w, (int64_t) w);
	}

	retval.add(s_MongoDBDriverManager_wmajority, !!(mongoc_write_concern_get_wmajority(write_concern)));
	retval.add(s_MongoDBDriverManager_wtimeout, (int64_t) mongoc_write_concern_get_wtimeout(write_concern));
	if (write_concern->fsync_ != MONGOC_WRITE_CONCERN_FSYNC_DEFAULT) {
		retval.add(s_MongoDBDriverManager_fsync, mongoc_write_concern_get_fsync(write_concern));
	} else {
		retval.add(s_MongoDBDriverManager_fsync, Variant());
	}
	if (write_concern->journal != MONGOC_WRITE_CONCERN_JOURNAL_DEFAULT) {
		retval.add(s_MongoDBDriverManager_journal, mongoc_write_concern_get_journal(write_concern));
	} else {
		retval.add(s_MongoDBDriverManager_journal, Variant());
	}

	return retval;
}

Object HHVM_METHOD(MongoDBDriverManager, selectServer, const Object &readPreference)
{
	MongoDBDriverManagerData* data = Native::data<MongoDBDriverManagerData>(this_);
	MongoDBDriverReadPreferenceData *rp_data = Native::data<MongoDBDriverReadPreferenceData>(readPreference.get());
	uint32_t server_id;

	server_id = mongoc_cluster_preselect(&data->m_client->cluster, MONGOC_OPCODE_QUERY, rp_data->m_read_preference, NULL);

	return hippo_mongo_driver_server_create_from_id(data->m_client, server_id);
}

}
