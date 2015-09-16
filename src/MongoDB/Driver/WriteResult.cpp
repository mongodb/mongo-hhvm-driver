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
#include "hphp/runtime/base/execution-context.h"

#include "../../../bson.h"
#include "../../../mongodb.h"
#include "../../../utils.h"

#define MONGOC_I_AM_A_DRIVER
#include "../../../libmongoc/src/mongoc/mongoc-bulk-operation-private.h"
#include "../../../libmongoc/src/mongoc/mongoc-write-concern-private.h"
#undef MONGOC_I_AM_A_DRIVER

#include "Server.h"
#include "WriteConcern.h"
#include "WriteResult.h"

namespace HPHP {

const StaticString s_MongoDriverWriteResult_className("MongoDB\\Driver\\WriteResult");
Class* MongoDBDriverWriteResultData::s_class = nullptr;
const StaticString MongoDBDriverWriteResultData::s_className("MongoDBDriverWriteResult");
IMPLEMENT_GET_CLASS(MongoDBDriverWriteResultData);

const StaticString s_MongoDriverWriteConcernError_className("MongoDB\\Driver\\WriteConcernError");

const HPHP::StaticString s_MongoDriverExceptionBulkWriteException_writeResult("writeResult");

Object HHVM_METHOD(MongoDBDriverWriteResult, getServer)
{
	MongoDBDriverWriteResultData* data = Native::data<MongoDBDriverWriteResultData>(this_);

	return hippo_mongo_driver_server_create_from_id(data->m_client, data->m_server_id);
}

bool HHVM_METHOD(MongoDBDriverWriteResult, isAcknowledged)
{
	MongoDBDriverWriteResultData* data = Native::data<MongoDBDriverWriteResultData>(this_);

	return !!_mongoc_write_concern_needs_gle(data->m_write_concern);
}

bool hippo_writeresult_get_write_errors(mongoc_write_result_t *writeresult, Object *errorObject)
{   
	const char *err = NULL;
	uint32_t code = 0;

	bson_iter_t iter;
	bson_iter_t citer;
	if (
		!bson_empty0(&writeresult->writeErrors) &&
		bson_iter_init(&iter, &writeresult->writeErrors) &&
		bson_iter_next(&iter) &&
		BSON_ITER_HOLDS_DOCUMENT(&iter) &&
		bson_iter_recurse(&iter, &citer)
	) {
		while (bson_iter_next(&citer)) { 
			if (BSON_ITER_IS_KEY(&citer, "errmsg")) {
				err = bson_iter_utf8(&citer, NULL);
			} else if (BSON_ITER_IS_KEY(&citer, "code")) {
				code = bson_iter_int32(&citer);
			}
		}

		*errorObject = MongoDriver::Utils::throwWriteException((char*) err, (int64_t) code);
		return true;
	}

	return false;
}

bool hippo_writeresult_get_writeconcern_error(mongoc_write_result_t *writeresult, Object *errorObject)
{
	const char *err = NULL;
	uint32_t code = 0;
			
	if (!bson_empty0(&writeresult->writeConcernError)) {
		bson_iter_t iter;

		if (bson_iter_init_find(&iter, &writeresult->writeConcernError, "code") && BSON_ITER_HOLDS_INT32(&iter)) {
			code = bson_iter_int32(&iter);
		}
		if (bson_iter_init_find(&iter, &writeresult->writeConcernError, "errmsg") && BSON_ITER_HOLDS_UTF8(&iter)) {
			err = bson_iter_utf8(&iter, NULL);
		}

		*errorObject = MongoDriver::Utils::throwWriteConcernException((char*) err, (int64_t) code);
		return true;
	}

	return false;
}

const StaticString
	s_nUpserted("nUpserted"),
	s_nMatched("nMatched"),
	s_nRemoved("nRemoved"),
	s_nInserted("nInserted"),
	s_nModified("nModified"),
	s_omit_nModified("omit_nModified"),
	s_writeConcern("writeConcern"),
	s_upsertedIds("upsertedIds"),
	s_writeErrors("writeErrors"),
	s_errmsg("errmsg"),
	s_message("message"),
	s_code("code"),
	s_info("info"),
	s_writeConcernError("writeConcernError");

Object hippo_write_result_init(mongoc_write_result_t *write_result, mongoc_client_t *client, int server_id, const mongoc_write_concern_t *write_concern, bool unwrap_bw_exception)
{
	static Class* c_writeResult;

	c_writeResult = Unit::lookupClass(s_MongoDriverWriteResult_className.get());
	assert(c_writeResult);
	Object obj = Object{c_writeResult};

	MongoDBDriverWriteResultData* wr_data = Native::data<MongoDBDriverWriteResultData>(obj.get());
	wr_data->m_client = client;
	wr_data->m_server_id = server_id;
	wr_data->m_write_concern = mongoc_write_concern_copy(write_concern);

	obj->o_set(s_nUpserted, Variant((int64_t) write_result->nUpserted), s_MongoDriverWriteResult_className);
	obj->o_set(s_nMatched, Variant((int64_t) write_result->nMatched), s_MongoDriverWriteResult_className);
	obj->o_set(s_nRemoved, Variant((int64_t) write_result->nRemoved), s_MongoDriverWriteResult_className);
	obj->o_set(s_nInserted, Variant((int64_t) write_result->nInserted), s_MongoDriverWriteResult_className);
	obj->o_set(s_nModified, Variant((int64_t) write_result->nModified), s_MongoDriverWriteResult_className);
	obj->o_set(s_omit_nModified, Variant((int64_t) write_result->omit_nModified), s_MongoDriverWriteResult_className);
/*
	obj->o_set(String("offset"), Variant((int64_t) write_result->offset), s_MongoDriverWriteResult_className);
	obj->o_set(String("n_commands"), Variant((int64_t) write_result->n_commands), s_MongoDriverWriteResult_className);
*/
	if (write_concern) {
		auto writeConcern_class = Unit::lookupClass(s_MongoDriverWriteConcern_className.get());
		auto writeConcern = Object{writeConcern_class};
		MongoDBDriverWriteConcernData* data = Native::data<MongoDBDriverWriteConcernData>(writeConcern);
		data->m_write_concern = mongoc_write_concern_copy(write_concern);

		Variant debugInfoResult;
		{
			Func *m = writeConcern_class->lookupMethod(s_MongoDriverWriteConcern_debugInfo.get());

			TypedValue args[0];

			g_context->invokeFuncFew(
				debugInfoResult.asTypedValue(),
				m,
				writeConcern.get(),
				nullptr,
				0, args
			);
		}

		obj->o_set(s_writeConcern, debugInfoResult, s_MongoDriverWriteConcern_className);
	} else {
		obj->o_set(s_writeConcern, Variant(), s_MongoDriverWriteConcern_className);
	}

	Variant v;
	hippo_bson_conversion_options_t options = HIPPO_TYPEMAP_DEBUG_INITIALIZER;

	BsonToVariantConverter convertor(bson_get_data(&write_result->upserted), write_result->upserted.len, options);
	convertor.convert(&v);
	obj->o_set(s_upsertedIds, v.toArray(), s_MongoDriverWriteResult_className);

	if (!bson_empty0(&write_result->writeErrors)) {
		BsonToVariantConverter convertor(bson_get_data(&write_result->writeErrors), write_result->writeErrors.len, options);
		convertor.convert(&v);
		obj->o_set(s_writeErrors, v.toArray(), s_MongoDriverWriteResult_className);
	}

	if (!bson_empty0(&write_result->writeConcernError)) {
		Array a_v;

		BsonToVariantConverter convertor(bson_get_data(&write_result->writeConcernError), write_result->writeConcernError.len, options);
		convertor.convert(&v);

		a_v = v.toArray();
	
		static Class* c_writeConcernError;

		c_writeConcernError = Unit::lookupClass(s_MongoDriverWriteConcernError_className.get());
		assert(c_writeConcernError);
		Object wce_obj = Object{c_writeConcernError};
		
		if (a_v.exists(s_errmsg)) {
			wce_obj->o_set(s_message, a_v[s_errmsg], s_MongoDriverWriteConcernError_className);
		}
		if (a_v.exists(s_code)) {
			wce_obj->o_set(s_code, a_v[s_code], s_MongoDriverWriteConcernError_className);
		}
		if (a_v.exists(s_info)) {
			wce_obj->o_set(s_info, a_v[s_info], s_MongoDriverWriteConcernError_className);
		} else {
			wce_obj->o_set(s_info, Variant(), s_MongoDriverWriteConcernError_className);
		}

		obj->o_set(s_writeConcernError, Variant(wce_obj), s_MongoDriverWriteResult_className);
	}

	/* If the result is null (typically a server_id, but 0 in case there was an
	 * error) */
	if (server_id == 0) {
		if (
			bson_empty0(&write_result->writeErrors) &&
			bson_empty0(&write_result->writeConcernError)
		) {
			throw MongoDriver::Utils::throwExceptionFromBsonError(&write_result->error);
		} else {
			Object errorObject;

			if (unwrap_bw_exception && hippo_writeresult_get_writeconcern_error(write_result, &errorObject)) {
				errorObject->o_set(s_MongoDriverExceptionBulkWriteException_writeResult, obj, MongoDriver::s_MongoDriverExceptionBulkWriteException_className);

				throw errorObject;
			} else if (unwrap_bw_exception && hippo_writeresult_get_write_errors(write_result, &errorObject)) {
				errorObject->o_set(s_MongoDriverExceptionBulkWriteException_writeResult, obj, MongoDriver::s_MongoDriverExceptionBulkWriteException_className);

				throw errorObject;
			} else {
				auto bw_exception = MongoDriver::Utils::throwBulkWriteException("BulkWrite error");
				bw_exception->o_set(s_MongoDriverExceptionBulkWriteException_writeResult, obj, MongoDriver::s_MongoDriverExceptionBulkWriteException_className);

				throw bw_exception;
			}
		}
	}

	return obj;
}

}
