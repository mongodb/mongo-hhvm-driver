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

#include "Binary.h"

namespace HPHP {

const StaticString s_MongoBsonBinary_className("MongoDB\\BSON\\Binary");
const StaticString s_MongoBsonBinary_data("data");
const StaticString s_MongoBsonBinary_subType("subType");

ObjectData* createMongoBsonBinaryObject(const uint8_t *v_binary, size_t v_binary_len, bson_subtype_t v_subtype)
{
	static Class* c_binary;
	String s;
	unsigned char *data_s;

	s = String(v_binary_len, ReserveString);
	data_s = (unsigned char*) s.bufferSlice().ptr;
	memcpy(data_s, v_binary, v_binary_len);
	s.setSize(v_binary_len);

	c_binary = Unit::lookupClass(s_MongoBsonBinary_className.get());
	assert(c_binary);
	ObjectData* obj = ObjectData::newInstance(c_binary);

	obj->o_set(s_MongoBsonBinary_data, s, s_MongoBsonBinary_className.get());
	obj->o_set(s_MongoBsonBinary_subType, Variant(v_subtype), s_MongoBsonBinary_className.get());

	return obj;
}

Array HHVM_METHOD(MongoDBBsonBinary, __debugInfo)
{
	Array retval = Array::Create();

	retval.set(s_MongoBsonBinary_data, this_->o_get(s_MongoBsonBinary_data, false, s_MongoBsonBinary_className));

	return retval;
}

}
