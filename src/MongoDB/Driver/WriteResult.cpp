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

Object HHVM_METHOD(MongoDBDriverWriteResult, getServer)
{
	static Class* c_server;
	MongoDBDriverWriteResultData* data = Native::data<MongoDBDriverWriteResultData>(this_);

	/* Prepare result */
	c_server = Unit::lookupClass(s_MongoDriverServer_className.get());
	assert(c_server);
	ObjectData* obj = ObjectData::newInstance(c_server);

	MongoDBDriverServerData* result_data = Native::data<MongoDBDriverServerData>(obj);

	result_data->m_client = data->m_client;
	result_data->m_server_id = data->m_server_id;

	return Object(obj);
}

bool HHVM_METHOD(MongoDBDriverWriteResult, isAcknowledged)
{
	MongoDBDriverWriteResultData* data = Native::data<MongoDBDriverWriteResultData>(this_);

	return !!_mongoc_write_concern_needs_gle(data->m_write_concern);
}

ObjectData *hippo_write_result_init(mongoc_write_result_t *write_result, mongoc_client_t *client, int server_id, const mongoc_write_concern_t *write_concern)
{
	static Class* c_writeResult;

	c_writeResult = Unit::lookupClass(s_MongoDriverWriteResult_className.get());
	assert(c_writeResult);
	ObjectData* obj = ObjectData::newInstance(c_writeResult);

	MongoDBDriverWriteResultData* wr_data = Native::data<MongoDBDriverWriteResultData>(obj);
	wr_data->m_client = client;
	wr_data->m_server_id = server_id;
	wr_data->m_write_concern = mongoc_write_concern_copy(write_concern);

	obj->o_set(String("nUpserted"), Variant((int64_t) write_result->nUpserted), s_MongoDriverWriteResult_className.get());
	obj->o_set(String("nMatched"), Variant((int64_t) write_result->nMatched), s_MongoDriverWriteResult_className.get());
	obj->o_set(String("nRemoved"), Variant((int64_t) write_result->nRemoved), s_MongoDriverWriteResult_className.get());
	obj->o_set(String("nInserted"), Variant((int64_t) write_result->nInserted), s_MongoDriverWriteResult_className.get());
	obj->o_set(String("nModified"), Variant((int64_t) write_result->nModified), s_MongoDriverWriteResult_className.get());
	obj->o_set(String("omit_nModified"), Variant((int64_t) write_result->omit_nModified), s_MongoDriverWriteResult_className.get());
/*
	obj->o_set(String("offset"), Variant((int64_t) write_result->offset), s_MongoDriverWriteResult_className.get());
	obj->o_set(String("n_commands"), Variant((int64_t) write_result->n_commands), s_MongoDriverWriteResult_className.get());
*/
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

	obj->o_set(String("writeConcern"), debugInfoResult, s_MongoDriverWriteConcern_className.get());

	Variant v;
	hippo_bson_conversion_options_t options = HIPPO_TYPEMAP_INITIALIZER;

	BsonToVariantConverter convertor(bson_get_data(&write_result->upserted), write_result->upserted.len, options);
	convertor.convert(&v);
	obj->o_set(String("upsertedIds"), v, s_MongoDriverWriteResult_className);

	return obj;
}

}
