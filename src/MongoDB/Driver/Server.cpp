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

#include "../../../bson.h"
#include "../../../mongodb.h"
#include "../../../utils.h"

#include "Command.h"
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
		hippo_bson_conversion_options_t options = HIPPO_TYPEMAP_INITIALIZER;

		BsonToVariantConverter convertor(bson_get_data(&sd->last_is_master), sd->last_is_master.len, options);
		convertor.convert(&v);

		return v.toArray();
	}

	throw MongoDriver::Utils::CreateAndConstruct(MongoDriver::s_MongoDriverExceptionRuntimeException_className, HPHP::Variant("Failed to get server description, server likely gone"), HPHP::Variant((uint64_t) 0));
}

Object HHVM_METHOD(MongoDBDriverServer, executeCommand, const String &db, const Object &command, const Variant &readPreference)
{
	bson_t *bson;
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);

	auto zquery = command->o_get(String("command"), false, s_MongoDriverCommand_className);

	VariantToBsonConverter converter(zquery, HIPPO_BSON_NO_FLAGS);
	bson = bson_new();
	converter.convert(bson);

	return MongoDriver::Utils::doExecuteCommand(
		db.c_str(),
		data->m_client,
		bson,
		NULL
	);
}

Object HHVM_METHOD(MongoDBDriverServer, executeQuery, const String &ns, const Object &query, const Variant &ReadPreference)
{
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);

	return MongoDriver::Utils::doExecuteQuery(
		ns,
		data->m_client,
		query,
		NULL
	);
}

}
