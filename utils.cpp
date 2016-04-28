/**
 *  Copyright 2015 MongoDB, Inc.
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
#include "hphp/runtime/base/array-init.h"
#include "hphp/runtime/base/execution-context.h"
#include "hphp/runtime/vm/native-data.h"

#include "utils.h"
#include "mongodb.h"

#include "src/MongoDB/Driver/BulkWrite.h"
#include "src/MongoDB/Driver/Cursor.h"
#include "src/MongoDB/Driver/Query.h"
#include "src/MongoDB/Driver/ReadConcern.h"
#include "src/MongoDB/Driver/ReadPreference.h"
#include "src/MongoDB/Driver/WriteResult.h"

namespace MongoDriver
{

bool Utils::splitNamespace(HPHP::String ns, char **db, char **col)
{
	const char *input = ns.c_str();
	const char *dot = strchr(input, '.');                                      

	if (!dot) {                                                                         
		return false;        
	}                                                                            

	if (col) {                                                                        
		*col = strdup(input + (dot - input) + 1);          
	}            
	if (db) {    
		*db = strndup(input, dot - input);
	}                       

	return true;  
}

HPHP::Object Utils::CreateAndConstruct(HPHP::StaticString classname, const HPHP::Variant &message, const HPHP::Variant &code)
{
	static HPHP::Class* c_class;

	c_class = HPHP::Unit::lookupClass(classname.get());
	assert(c_class);
	HPHP::Object inst = HPHP::Object{c_class};

	HPHP::TypedValue ret;
	HPHP::g_context->invokeFunc(&ret, c_class->getCtor(), HPHP::make_packed_array(message, code), inst.get());
	HPHP::tvRefcountedDecRef(&ret);

	return inst;
}

const HPHP::StaticString s_MongoDriverExceptionAuthenticationException_className("MongoDB\\Driver\\Exception\\AuthenticationException");
const HPHP::StaticString s_MongoDriverExceptionBulkWriteException_className("MongoDB\\Driver\\Exception\\BulkWriteException");
const HPHP::StaticString s_MongoDriverExceptionConnectionException_className("MongoDB\\Driver\\Exception\\ConnectionException");
const HPHP::StaticString s_MongoDriverExceptionConnectionTimeoutException_className("MongoDB\\Driver\\Exception\\ConnectionTimeoutException");
const HPHP::StaticString s_MongoDriverExceptionExecutionTimeoutException_className("MongoDB\\Driver\\Exception\\ExecutionTimeoutException");
const HPHP::StaticString s_MongoDriverExceptionInvalidArgumentException_className("MongoDB\\Driver\\Exception\\InvalidArgumentException");
const HPHP::StaticString s_MongoDriverExceptionLogicException_className("MongoDB\\Driver\\Exception\\LogicException");
const HPHP::StaticString s_MongoDriverExceptionRuntimeException_className("MongoDB\\Driver\\Exception\\RuntimeException");
const HPHP::StaticString s_MongoDriverExceptionUnexpectedValueException_className("MongoDB\\Driver\\Exception\\UnexpectedValueException");
const HPHP::StaticString s_MongoDriverExceptionWriteException_className("MongoDB\\Driver\\Exception\\WriteException");

HPHP::Object Utils::throwInvalidArgumentException(char *errormessage)
{
	return Utils::CreateAndConstruct(s_MongoDriverExceptionInvalidArgumentException_className, HPHP::Variant(errormessage), HPHP::Variant((uint64_t) 0));
}

HPHP::Object Utils::throwInvalidArgumentException(HPHP::String errormessage)
{
	return Utils::CreateAndConstruct(s_MongoDriverExceptionInvalidArgumentException_className, HPHP::Variant(errormessage), HPHP::Variant((uint64_t) 0));
}

HPHP::Object Utils::throwBulkWriteException(HPHP::String errormessage)
{
	return Utils::CreateAndConstruct(s_MongoDriverExceptionBulkWriteException_className, HPHP::Variant(errormessage), HPHP::Variant((uint64_t) 0));
}

HPHP::Object Utils::throwLogicException(char *errormessage)
{
	return Utils::CreateAndConstruct(s_MongoDriverExceptionLogicException_className, HPHP::Variant(errormessage), HPHP::Variant((uint64_t) 0));
}

HPHP::Object Utils::throwRunTimeException(char *errormessage)
{
	return Utils::CreateAndConstruct(s_MongoDriverExceptionRuntimeException_className, HPHP::Variant(errormessage), HPHP::Variant((uint64_t) 0));
}

HPHP::Object Utils::throwRunTimeException(HPHP::String errormessage)
{
	return Utils::CreateAndConstruct(s_MongoDriverExceptionRuntimeException_className, HPHP::Variant(errormessage), HPHP::Variant((uint64_t) 0));
}

HPHP::Object Utils::throwUnexpectedValueException(char *errormessage)
{
	return Utils::CreateAndConstruct(s_MongoDriverExceptionUnexpectedValueException_className, HPHP::Variant(errormessage), HPHP::Variant((uint64_t) 0));
}

HPHP::Object Utils::throwUnexpectedValueException(HPHP::String errormessage)
{
	return Utils::CreateAndConstruct(s_MongoDriverExceptionUnexpectedValueException_className, HPHP::Variant(errormessage), HPHP::Variant((uint64_t) 0));
}

HPHP::Object Utils::throwExceptionFromBsonError(bson_error_t *error)
{
	switch (error->code) {
		case 50: /* ExceededTimeLimit */
			return Utils::CreateAndConstruct(s_MongoDriverExceptionExecutionTimeoutException_className, HPHP::Variant(error->message), HPHP::Variant((uint64_t) error->code));
		case MONGOC_ERROR_STREAM_SOCKET:
		case MONGOC_ERROR_SERVER_SELECTION_FAILURE:
			return Utils::CreateAndConstruct(s_MongoDriverExceptionConnectionTimeoutException_className, HPHP::Variant(error->message), HPHP::Variant((uint64_t) error->code));
		case MONGOC_ERROR_CLIENT_AUTHENTICATE:
			return Utils::CreateAndConstruct(s_MongoDriverExceptionAuthenticationException_className, HPHP::Variant(error->message), HPHP::Variant((uint64_t) error->code));

		case MONGOC_ERROR_STREAM_INVALID_TYPE:
		case MONGOC_ERROR_STREAM_INVALID_STATE:
		case MONGOC_ERROR_STREAM_NAME_RESOLUTION:
		case MONGOC_ERROR_STREAM_CONNECT:
		case MONGOC_ERROR_STREAM_NOT_ESTABLISHED:
			return Utils::CreateAndConstruct(s_MongoDriverExceptionConnectionException_className, HPHP::Variant(error->message), HPHP::Variant((uint64_t) error->code));
		case MONGOC_ERROR_CLIENT_NOT_READY:
		case MONGOC_ERROR_CLIENT_TOO_BIG:
		case MONGOC_ERROR_CLIENT_TOO_SMALL:
		case MONGOC_ERROR_CLIENT_GETNONCE:
		case MONGOC_ERROR_CLIENT_NO_ACCEPTABLE_PEER:
		case MONGOC_ERROR_CLIENT_IN_EXHAUST:
		case MONGOC_ERROR_PROTOCOL_INVALID_REPLY:
		case MONGOC_ERROR_PROTOCOL_BAD_WIRE_VERSION:
		case MONGOC_ERROR_CURSOR_INVALID_CURSOR:
		case MONGOC_ERROR_QUERY_FAILURE:
		/*case MONGOC_ERROR_PROTOCOL_ERROR:*/
		case MONGOC_ERROR_BSON_INVALID:
		case MONGOC_ERROR_MATCHER_INVALID:
		case MONGOC_ERROR_NAMESPACE_INVALID:
		case MONGOC_ERROR_COLLECTION_INSERT_FAILED:
		case MONGOC_ERROR_GRIDFS_INVALID_FILENAME:
		case MONGOC_ERROR_QUERY_COMMAND_NOT_FOUND:
		case MONGOC_ERROR_QUERY_NOT_TAILABLE:
			return Utils::CreateAndConstruct(s_MongoDriverExceptionRuntimeException_className, HPHP::Variant(error->message), HPHP::Variant((uint64_t) error->code));

		case MONGOC_ERROR_COMMAND_INVALID_ARG:
			return Utils::CreateAndConstruct(s_MongoDriverExceptionInvalidArgumentException_className, HPHP::Variant("Cannot execute an empty BulkWrite"), HPHP::Variant((uint64_t) 0));
	}
	switch (error->domain) {
		case MONGOC_ERROR_CLIENT:
		case MONGOC_ERROR_STREAM:
		case MONGOC_ERROR_PROTOCOL:
		case MONGOC_ERROR_CURSOR:
		case MONGOC_ERROR_QUERY:
		case MONGOC_ERROR_INSERT:
		case MONGOC_ERROR_SASL:
		case MONGOC_ERROR_BSON:
		case MONGOC_ERROR_MATCHER:
		case MONGOC_ERROR_NAMESPACE:
		case MONGOC_ERROR_COMMAND:
		case MONGOC_ERROR_COLLECTION:
		case MONGOC_ERROR_GRIDFS:
			/* FIXME: We don't have the Exceptions mocked yet.. */
#if 0
			return phongo_ce_mongo_connection_exception;
#endif
		default:
			return Utils::CreateAndConstruct(s_MongoDriverExceptionRuntimeException_className, HPHP::Variant(error->message), HPHP::Variant((uint64_t) error->code));
	}
}

HPHP::Object Utils::doExecuteBulkWrite(const HPHP::String ns, mongoc_client_t *client, int server_id, const HPHP::Object bulk, const mongoc_write_concern_t *write_concern)
{
	HPHP::MongoDBDriverBulkWriteData* bulk_data = HPHP::Native::data<HPHP::MongoDBDriverBulkWriteData>(bulk.get());
	bson_error_t error;
	int success;
	bson_t reply = BSON_INITIALIZER;

	/* Free previous database/collection, until PHPC-676 and HHVM-222 are resolved */
	if (bulk_data->m_database) {
		free(bulk_data->m_database);
	}
	if (bulk_data->m_collection) {
		free(bulk_data->m_collection);
	}

	/* Prepare */
	if (!MongoDriver::Utils::splitNamespace(ns, &bulk_data->m_database, &bulk_data->m_collection)) {
		throw throwInvalidArgumentException("Invalid namespace provided: " + ns);
	}

	/* Setup operation */
	mongoc_bulk_operation_set_database(bulk_data->m_bulk, bulk_data->m_database);
	mongoc_bulk_operation_set_collection(bulk_data->m_bulk, bulk_data->m_collection);
	mongoc_bulk_operation_set_client(bulk_data->m_bulk, client);

	/* Deal with write concerns */
	if (write_concern) {
		mongoc_bulk_operation_set_write_concern(bulk_data->m_bulk, write_concern);
	} else {
		write_concern = mongoc_client_get_write_concern(client);
	}

	/* Handle server hint */
	if (server_id > 0) {
		mongoc_bulk_operation_set_hint(bulk_data->m_bulk, server_id);
	}

	/* Run operation */
	success = mongoc_bulk_operation_execute(bulk_data->m_bulk, &reply, &error);
	bulk_data->m_executed = true;

	/* Prepare result */
	HPHP::Object obj = HPHP::hippo_write_result_init(&reply, &error, client, mongoc_bulk_operation_get_hint(bulk_data->m_bulk), success, write_concern);
	bson_destroy(&reply);

	return obj;
}

/* Advance the cursor and return whether there is an error. On error, the
 * cursor will be destroyed and an exception will be thrown. */
static void hippo_advance_cursor_and_check_for_error(mongoc_cursor_t *cursor)
{
	const bson_t *doc;

	/* Check for errors */
	if (!mongoc_cursor_next(cursor, &doc)) {
		bson_error_t error;

		/* Could simply be no docs, which is not an error */
		if (mongoc_cursor_error(cursor, &error)) {
			mongoc_cursor_destroy(cursor);
			throw Utils::throwExceptionFromBsonError(&error);
		}
	}
}

HPHP::Object Utils::doExecuteCommand(const char *db, mongoc_client_t *client, int server_id, bson_t *command, HPHP::Variant readPreference)
{
	mongoc_cursor_t *cursor;
	bson_iter_t iter;
	mongoc_read_prefs_t *read_preference = NULL;

	if (!readPreference.isNull()) {
		HPHP::Object o_rp = readPreference.toObject();
		HPHP::MongoDBDriverReadPreferenceData* data = HPHP::Native::data<HPHP::MongoDBDriverReadPreferenceData>(o_rp);

		read_preference = data->m_read_preference;
	}

	/* Run operation */
	cursor = mongoc_client_command(client, db, MONGOC_QUERY_NONE, 0, 1, 0, command, NULL, read_preference);

	/* Handle server hint */
	if (server_id > 0) {
		cursor->server_id = server_id;
	}

	/* This throws an exception upon error */
	hippo_advance_cursor_and_check_for_error(cursor);

	if (bson_iter_init_find(&iter, mongoc_cursor_current(cursor), "cursor") && BSON_ITER_HOLDS_DOCUMENT(&iter)) {
		mongoc_cursor_t *cmd_cursor;

		/* According to mongoc_cursor_new_from_command_reply(), the reply bson_t
		 * is ultimately destroyed on both success and failure. Use bson_copy()
		 * to create a writable copy of the const bson_t we fetched above. */
		cmd_cursor = mongoc_cursor_new_from_command_reply(client, bson_copy(mongoc_cursor_current(cursor)), mongoc_cursor_get_hint(cursor));
		mongoc_cursor_destroy(cursor);

		/* This throws an exception upon error */
		hippo_advance_cursor_and_check_for_error(cmd_cursor);

		return HPHP::hippo_cursor_init(cmd_cursor, client);
	}

	/* Prepare result */
	HPHP::Object obj = HPHP::hippo_cursor_init(cursor, client);

	/* Destroy */
	bson_destroy(command);

	return obj;
}

const HPHP::StaticString
	s_query("query"),
	s_skip("skip"),
	s_limit("limit"),
	s_batchSize("batchSize"),
	s_flags("flags"),
	s_fields("fields"),
	s_readConcern("readConcern");

HPHP::Object Utils::doExecuteQuery(const HPHP::String ns, mongoc_client_t *client, int server_id, HPHP::Object query, HPHP::Variant readPreference)
{
	bson_t *bson_query = NULL, *bson_fields = NULL;
	mongoc_collection_t *collection;
	mongoc_cursor_t *cursor;

	uint32_t skip, limit, batch_size;
	mongoc_query_flags_t flags;
	char *dbname;
	char *collname;

	mongoc_read_prefs_t *read_preference = NULL;

	/* Prepare */
	if (!MongoDriver::Utils::splitNamespace(ns, &dbname, &collname)) {
		throw throwInvalidArgumentException("Invalid namespace provided: " + ns);
	}

	/* Get query properties */
	auto zquery = query->o_get(s_query, false, HPHP::s_MongoDriverQuery_className);

	if (zquery.getType() == HPHP::KindOfArray) {
		const HPHP::Array& aquery = zquery.toArray();

		skip = aquery[s_skip].toInt32();
		limit = aquery[s_limit].toInt32();
		batch_size = aquery[s_batchSize].toInt32();
		flags = (mongoc_query_flags_t) aquery[s_flags].toInt32();

		HPHP::VariantToBsonConverter converter(aquery[s_query], HIPPO_BSON_NO_FLAGS);
		bson_query = bson_new();
		converter.convert(bson_query);

		if (aquery.exists(s_fields)) {
			HPHP::VariantToBsonConverter converter(aquery[s_fields], HIPPO_BSON_NO_FLAGS);
			bson_fields = bson_new();
			converter.convert(bson_fields);
		}

		if (aquery.exists(s_readConcern)) {
			mongoc_read_concern_t *rc;

			rc = mongoc_read_concern_new();
			mongoc_read_concern_set_level(rc, aquery[s_readConcern].toString().c_str());

			mongoc_client_set_read_concern(client, rc);
		}
	}

	if (!readPreference.isNull()) {
		HPHP::Object o_rp = readPreference.toObject();
		HPHP::MongoDBDriverReadPreferenceData* data = HPHP::Native::data<HPHP::MongoDBDriverReadPreferenceData>(o_rp);

		read_preference = data->m_read_preference;
	}

	/* Run query and get cursor */
	collection = mongoc_client_get_collection(client, dbname, collname);
	cursor = mongoc_collection_find(collection, flags, skip, limit, batch_size, bson_query, bson_fields, read_preference);
	mongoc_collection_destroy(collection);

	/* Handle server hint */
	if (server_id > 0) {
		cursor->server_id = server_id;
	}

	/* This throws an exception upon error */
	hippo_advance_cursor_and_check_for_error(cursor);

	/* Prepare result */
	return HPHP::hippo_cursor_init(cursor, client);
}


}
