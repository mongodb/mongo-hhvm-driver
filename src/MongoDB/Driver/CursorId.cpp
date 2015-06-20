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

#include "CursorId.h"

namespace HPHP {

const StaticString s_MongoDriverCursorId_className("MongoDB\\Driver\\CursorId");
Class* MongoDBDriverCursorIdData::s_class = nullptr;
const StaticString MongoDBDriverCursorIdData::s_className("MongoDBDriverCursorId");
IMPLEMENT_GET_CLASS(MongoDBDriverCursorIdData);

void HHVM_METHOD(MongoDBDriverCursorId, __construct, const String &id)
{
	MongoDBDriverCursorIdData* data = Native::data<MongoDBDriverCursorIdData>(this_);

	data->id = id.toInt64();
}

String HHVM_METHOD(MongoDBDriverCursorId, __toString)
{
	MongoDBDriverCursorIdData* data = Native::data<MongoDBDriverCursorIdData>(this_);

	return String(data->id);
}

}
