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

#include "ObjectId.h"

namespace HPHP {

const StaticString s_MongoBsonObjectId_className("BSON\\ObjectId");
Class* MongoDBBsonObjectIdData::s_class = nullptr;
const StaticString MongoDBBsonObjectIdData::s_className("MongoDBBsonObjectId");
IMPLEMENT_GET_CLASS(MongoDBBsonObjectIdData);

void HHVM_METHOD(MongoDBBsonObjectId, __construct, const Variant &objectId)
{
	MongoDBBsonObjectIdData* data = Native::data<MongoDBBsonObjectIdData>(this_);
	const String& str_objectId = objectId.isNull() ? null_string : objectId.toString();

	if (!objectId.isNull()) {
		if (bson_oid_is_valid(str_objectId.c_str(), str_objectId.length())) {
			bson_oid_init_from_string(&data->m_oid, str_objectId.c_str());
		} else {
			throw Object(SystemLib::AllocInvalidArgumentExceptionObject("Invalid BSON ID provided"));
		}
	} else {
		bson_oid_init(&data->m_oid, NULL);
	}
}

String HHVM_METHOD(MongoDBBsonObjectId, __toString)
{
	String s;
	char *data_s;
	MongoDBBsonObjectIdData* data = Native::data<MongoDBBsonObjectIdData>(this_);

	s = String(24, ReserveString);
	data_s = s.bufferSlice().ptr;
	bson_oid_to_string(&data->m_oid, data_s);
	s.setSize(24);

	return s;
}

}
