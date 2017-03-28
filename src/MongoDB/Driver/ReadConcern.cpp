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
#include "../../../utils.h"

#include "ReadConcern.h"

namespace HPHP {

const StaticString s_MongoDriverReadConcern_className("MongoDB\\Driver\\ReadConcern");
const StaticString s_MongoDriverReadConcern_local(MONGOC_READ_CONCERN_LEVEL_LOCAL);
const StaticString s_MongoDriverReadConcern_majority(MONGOC_READ_CONCERN_LEVEL_MAJORITY);
const StaticString s_MongoDriverReadConcern_linearizable(MONGOC_READ_CONCERN_LEVEL_LINEARIZABLE);
Class* MongoDBDriverReadConcernData::s_class = nullptr;
const StaticString MongoDBDriverReadConcernData::s_className("MongoDBDriverReadConcern");
IMPLEMENT_GET_CLASS(MongoDBDriverReadConcernData);

void HHVM_METHOD(MongoDBDriverReadConcern, __construct, const Variant &level)
{
	MongoDBDriverReadConcernData* data = Native::data<MongoDBDriverReadConcernData>(this_);

	data->m_read_concern = mongoc_read_concern_new();

	if (!level.isNull()) {
		mongoc_read_concern_set_level(data->m_read_concern, level.toString().c_str());
	}
}

const StaticString
	s_level("level");


Array HHVM_METHOD(MongoDBDriverReadConcern, __debugInfo)
{
	MongoDBDriverReadConcernData* data = Native::data<MongoDBDriverReadConcernData>(this_);
	Array retval = Array::Create();
	const char *level;

	level = mongoc_read_concern_get_level(data->m_read_concern);

	if (level) {
		retval.set(s_level, Variant(level));
	}

	return retval;
}

Variant HHVM_METHOD(MongoDBDriverReadConcern, bsonSerialize)
{
	Array retval = HHVM_MN(MongoDBDriverReadConcern, __debugInfo)(this_);
	return Variant(Variant(retval).toObject());
}

Variant HHVM_METHOD(MongoDBDriverReadConcern, getLevel)
{
	MongoDBDriverReadConcernData* data = Native::data<MongoDBDriverReadConcernData>(this_);
	const char *level;

	level = mongoc_read_concern_get_level(data->m_read_concern);

	if (level) {
		return Variant(level);
	}

	return Variant();
}

}
