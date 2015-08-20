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
#include "WriteConcern.h"

namespace HPHP {

const StaticString s_MongoDriverServer_className("MongoDB\\Driver\\Server");
Class* MongoDBDriverServerData::s_class = nullptr;
const StaticString MongoDBDriverServerData::s_className("MongoDBDriverServer");
const StaticString s_MongoDriverServer_host("host");
const StaticString s_MongoDriverServer_port("port");
const StaticString s_MongoDriverServer_type("type");
const StaticString s_MongoDriverServer_is_primary("is_primary");
const StaticString s_MongoDriverServer_is_secondary("is_secondary");
const StaticString s_MongoDriverServer_is_arbiter("is_arbiter");
const StaticString s_MongoDriverServer_hidden("hidden");
const StaticString s_MongoDriverServer_passive("passive");
const StaticString s_MongoDriverServer_is_hidden("is_hidden");
const StaticString s_MongoDriverServer_is_passive("is_passive");
const StaticString s_MongoDriverServer_tags("tags");
const StaticString s_MongoDriverServer_last_is_master("last_is_master");
const StaticString s_MongoDriverServer_round_trip_time("round_trip_time");
IMPLEMENT_GET_CLASS(MongoDBDriverServerData);

Array HHVM_METHOD(MongoDBDriverServer, __debugInfo)
{
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);
	mongoc_server_description_t *sd;

	Array retval = Array::Create();

	if ((sd = mongoc_topology_description_server_by_id(&data->m_client->topology->description, data->m_server_id))) {
		Variant v_last_is_master;
		Array   a_last_is_master;

		retval.set(s_MongoDriverServer_host, sd->host.host);
		retval.set(s_MongoDriverServer_port, sd->host.port);
		retval.set(s_MongoDriverServer_type, sd->type);
		retval.set(s_MongoDriverServer_is_primary, !!(sd->type == MONGOC_SERVER_RS_PRIMARY));
		retval.set(s_MongoDriverServer_is_secondary, !!(sd->type == MONGOC_SERVER_RS_SECONDARY));
		retval.set(s_MongoDriverServer_is_arbiter, !!(sd->type == MONGOC_SERVER_RS_ARBITER));

		hippo_bson_conversion_options_t options = HIPPO_TYPEMAP_DEBUG_INITIALIZER;
		BsonToVariantConverter convertor(bson_get_data(&sd->last_is_master), sd->last_is_master.len, options);
		convertor.convert(&v_last_is_master);
		a_last_is_master = v_last_is_master.toArray();

		retval.set(s_MongoDriverServer_is_hidden, a_last_is_master.exists(s_MongoDriverServer_hidden) && !!a_last_is_master[s_MongoDriverServer_hidden].toBoolean());
		retval.set(s_MongoDriverServer_is_passive, a_last_is_master.exists(s_MongoDriverServer_passive) && !!a_last_is_master[s_MongoDriverServer_passive].toBoolean());
		retval.set(s_MongoDriverServer_last_is_master, a_last_is_master);

		retval.set(s_MongoDriverServer_round_trip_time, sd->round_trip_time);

		return retval;
	}

	throw MongoDriver::Utils::CreateAndConstruct(MongoDriver::s_MongoDriverExceptionRuntimeException_className, HPHP::Variant("Failed to get server description, server likely gone"), HPHP::Variant((uint64_t) 0));
}

String HHVM_METHOD(MongoDBDriverServer, getHost)
{
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);
	mongoc_server_description_t *sd;

	if ((sd = mongoc_topology_description_server_by_id(&data->m_client->topology->description, data->m_server_id))) {
		return String(sd->host.host);
	}

	throw MongoDriver::Utils::CreateAndConstruct(MongoDriver::s_MongoDriverExceptionRuntimeException_className, HPHP::Variant("Failed to get server description, server likely gone"), HPHP::Variant((uint64_t) 0));
}

Array HHVM_METHOD(MongoDBDriverServer, getInfo)
{
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);
	mongoc_server_description_t *sd;

	if ((sd = mongoc_topology_description_server_by_id(&data->m_client->topology->description, data->m_server_id))) {
		Variant v;
		hippo_bson_conversion_options_t options = HIPPO_TYPEMAP_DEBUG_INITIALIZER;

		BsonToVariantConverter convertor(bson_get_data(&sd->last_is_master), sd->last_is_master.len, options);
		convertor.convert(&v);

		return v.toArray();
	}

	throw MongoDriver::Utils::CreateAndConstruct(MongoDriver::s_MongoDriverExceptionRuntimeException_className, HPHP::Variant("Failed to get server description, server likely gone"), HPHP::Variant((uint64_t) 0));
}

int64_t HHVM_METHOD(MongoDBDriverServer, getLatency)
{
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);
	mongoc_server_description_t *sd;

	if ((sd = mongoc_topology_description_server_by_id(&data->m_client->topology->description, data->m_server_id))) {
		return (int64_t) (sd->round_trip_time);
	}

	throw MongoDriver::Utils::CreateAndConstruct(MongoDriver::s_MongoDriverExceptionRuntimeException_className, HPHP::Variant("Failed to get server description, server likely gone"), HPHP::Variant((uint64_t) 0));
}

int64_t HHVM_METHOD(MongoDBDriverServer, getPort)
{
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);
	mongoc_server_description_t *sd;

	if ((sd = mongoc_topology_description_server_by_id(&data->m_client->topology->description, data->m_server_id))) {
		return (int64_t) (sd->host.port);
	}

	throw MongoDriver::Utils::CreateAndConstruct(MongoDriver::s_MongoDriverExceptionRuntimeException_className, HPHP::Variant("Failed to get server description, server likely gone"), HPHP::Variant((uint64_t) 0));
}

Array HHVM_METHOD(MongoDBDriverServer, getTags)
{
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);
	mongoc_server_description_t *sd;

	if ((sd = mongoc_topology_description_server_by_id(&data->m_client->topology->description, data->m_server_id))) {
		Variant v;
		hippo_bson_conversion_options_t options = HIPPO_TYPEMAP_DEBUG_INITIALIZER;

		BsonToVariantConverter convertor(bson_get_data(&sd->tags), sd->tags.len, options);
		convertor.convert(&v);

		return v.toArray();
	}

	throw MongoDriver::Utils::CreateAndConstruct(MongoDriver::s_MongoDriverExceptionRuntimeException_className, HPHP::Variant("Failed to get server description, server likely gone"), HPHP::Variant((uint64_t) 0));
}

int64_t HHVM_METHOD(MongoDBDriverServer, getType)
{
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);
	mongoc_server_description_t *sd;

	if ((sd = mongoc_topology_description_server_by_id(&data->m_client->topology->description, data->m_server_id))) {
		return (int64_t) (sd->type);
	}

	throw MongoDriver::Utils::CreateAndConstruct(MongoDriver::s_MongoDriverExceptionRuntimeException_className, HPHP::Variant("Failed to get server description, server likely gone"), HPHP::Variant((uint64_t) 0));
}

bool HHVM_METHOD(MongoDBDriverServer, isPrimary)
{
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);
	mongoc_server_description_t *sd;

	if ((sd = mongoc_topology_description_server_by_id(&data->m_client->topology->description, data->m_server_id))) {
		return (bool) (sd->type == MONGOC_SERVER_RS_PRIMARY);
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
		data->m_server_id,
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
		data->m_server_id,
		query,
		NULL
	);
}

Object HHVM_METHOD(MongoDBDriverServer, executeBulkWrite, const String &ns, const Object &bulk, const Variant &writeConcern)
{
	const mongoc_write_concern_t *write_concern = NULL;
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);

	/* Deal with write concerns */
	if (!writeConcern.isNull()) {
		MongoDBDriverWriteConcernData* wc_data = Native::data<MongoDBDriverWriteConcernData>(writeConcern.toObject().get());
		write_concern = wc_data->m_write_concern;
	}
	if (!write_concern) {
		write_concern = mongoc_client_get_write_concern(data->m_client);
	}

	return MongoDriver::Utils::doExecuteBulkWrite(
		ns,
		data->m_client,
		data->m_server_id,
		bulk,
		write_concern
	);
}

}
