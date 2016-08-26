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
#include "hphp/runtime/ext/string/ext_string.h"

#undef TRACE

#include "../../../mongodb.h"
#include "../../../utils.h"

#include "ObjectID.h"

namespace HPHP {

const StaticString s_MongoBsonObjectID_className("MongoDB\\BSON\\ObjectID");
const StaticString s_MongoBsonObjectID_oid("oid");
Class* MongoDBBsonObjectIDData::s_class = nullptr;
const StaticString MongoDBBsonObjectIDData::s_className("MongoDBBsonObjectID");
IMPLEMENT_GET_CLASS(MongoDBBsonObjectIDData);

static String oidAsString(MongoDBBsonObjectIDData* data)
{
	String s;
	char *data_s;

	s = String(24, ReserveString);
	data_s = s.bufferSlice().data();
	bson_oid_to_string(&data->m_oid, data_s);
	s.setSize(24);

	return s;
}

void HHVM_METHOD(MongoDBBsonObjectID, __construct, const Variant &objectId)
{
	MongoDBBsonObjectIDData* data = Native::data<MongoDBBsonObjectIDData>(this_);
	const String& str_objectId = objectId.isNull() ? null_string : objectId.toString();

	if (!objectId.isNull()) {
		if (bson_oid_is_valid((HHVM_FN(strtolower)(str_objectId)).c_str(), str_objectId.length())) {
			bson_oid_init_from_string(&data->m_oid, (HHVM_FN(strtolower)(str_objectId)).c_str());
		} else {
			throw MongoDriver::Utils::throwInvalidArgumentException("Error parsing ObjectID string: " + str_objectId);
		}
	} else {
		bson_oid_init(&data->m_oid, NULL);
	}

	this_->o_set(s_MongoBsonObjectID_oid, Variant(oidAsString(data)), s_MongoBsonObjectID_className);
}

String HHVM_METHOD(MongoDBBsonObjectID, __toString)
{
	MongoDBBsonObjectIDData* data = Native::data<MongoDBBsonObjectIDData>(this_);

	return oidAsString(data);
}

const StaticString s_oid("oid");

Array HHVM_METHOD(MongoDBBsonObjectID, __debugInfo)
{
	Array retval = Array::Create();
	MongoDBBsonObjectIDData* data = Native::data<MongoDBBsonObjectIDData>(this_);

	retval.set(s_oid, oidAsString(data));

	return retval;
}

}
