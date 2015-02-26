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

#include "../../../mongodb.h"

#include "QueryResult.h"

namespace HPHP {

const StaticString s_MongoDriverQueryResult_className("MongoDB\\Driver\\QueryResult");
Class* MongoDBDriverQueryResultData::s_class = nullptr;
const StaticString MongoDBDriverQueryResultData::s_className("MongoDBDriverQueryResult");
IMPLEMENT_GET_CLASS(MongoDBDriverQueryResultData);

Object HHVM_METHOD(MongoDBDriverQueryResult, getIterator)
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
	cursor_data->current = 0;
	cursor_data->visitor_data.zchild = NULL;

	return Object(obj);
}

Object HHVM_METHOD(MongoDBDriverQueryResult, getServer)
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

}
