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

#include "../../../mongodb.h"

#define MONGOC_I_AM_A_DRIVER
#define delete not_delete
#include "../../../libmongoc/src/mongoc/mongoc-bulk-operation-private.h"
#include "../../../libmongoc/src/mongoc/mongoc-write-concern-private.h"
#undef delete
#undef MONGOC_I_AM_A_DRIVER

#include "WriteResult.h"

namespace HPHP {

const StaticString s_MongoDriverWriteResult_className("MongoDB\\Driver\\WriteResult");
Class* MongoDBDriverWriteResultData::s_class = nullptr;
const StaticString MongoDBDriverWriteResultData::s_className("MongoDBDriverWriteResult");
IMPLEMENT_GET_CLASS(MongoDBDriverWriteResultData);

Object HHVM_METHOD(MongoDBDriverWriteResult, getServer)
{
}

bool HHVM_METHOD(MongoDBDriverWriteResult, isAcknowledged)
{
	MongoDBDriverWriteResultData* data = Native::data<MongoDBDriverWriteResultData>(this_);

	return !!_mongoc_write_concern_needs_gle(data->m_write_concern);
}

}
