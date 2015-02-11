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

#include "CursorId.h"
#include "Cursor.h"

namespace HPHP {

const StaticString s_MongoDriverCursor_className("MongoDB\\Driver\\Cursor");
Class* MongoDBDriverCursorData::s_class = nullptr;
const StaticString MongoDBDriverCursorData::s_className("MongoDBDriverCursor");
IMPLEMENT_GET_CLASS(MongoDBDriverCursorData);

Object HHVM_METHOD(MongoDBDriverCursor, getId)
{
	static Class* c_cursor;
	int64_t cursorid;
	MongoDBDriverCursorData* data = Native::data<MongoDBDriverCursorData>(this_);

	cursorid = mongoc_cursor_get_id(data->cursor);

	/* Prepare result */
	c_cursor = Unit::lookupClass(s_MongoDriverCursorId_className.get());
	assert(c_cursor);
	ObjectData* obj = ObjectData::newInstance(c_cursor);

	MongoDBDriverCursorIdData* cursorid_data = Native::data<MongoDBDriverCursorIdData>(obj);

	cursorid_data->id = cursorid;

	return Object(obj);
}

}
