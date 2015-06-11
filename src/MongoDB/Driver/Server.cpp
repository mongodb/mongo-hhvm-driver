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

#include "../../../bson.h"
#include "../../../mongodb.h"
#include "../../../utils.h"

#include "Server.h"

namespace HPHP {

const StaticString s_MongoDriverServer_className("MongoDB\\Driver\\Server");
Class* MongoDBDriverServerData::s_class = nullptr;
const StaticString MongoDBDriverServerData::s_className("MongoDBDriverServer");
IMPLEMENT_GET_CLASS(MongoDBDriverServerData);

Array HHVM_METHOD(MongoDBDriverServer, getInfo)
{
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);
	mongoc_server_description_t *sd;

	if ((sd = mongoc_topology_description_server_by_id(&data->m_client->topology->description, data->m_server_id))) {
		Variant v;

		BsonToVariantConverter convertor(bson_get_data(&sd->last_is_master), sd->last_is_master.len, NULL);
		convertor.convert(&v);

		return v.toArray();
	}

	throw MongoDriver::Utils::CreateAndConstruct(MongoDriver::s_MongoDriverExceptionRuntimeException_className, HPHP::Variant("Failed to get server description, server likely gone"), HPHP::Variant((uint64_t) 0));
}

}
