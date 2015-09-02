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
IMPLEMENT_GET_CLASS(MongoDBDriverServerData);

const StaticString
	s_MongoDriverServer___serverId("__serverId"),
	s_MongoDriverServer_host("host"),
	s_MongoDriverServer_port("port"),
	s_MongoDriverServer_type("type"),
	s_MongoDriverServer_is_primary("is_primary"),
	s_MongoDriverServer_is_secondary("is_secondary"),
	s_MongoDriverServer_is_arbiter("is_arbiter"),
	s_MongoDriverServer_hidden("hidden"),
	s_MongoDriverServer_passive("passive"),
	s_MongoDriverServer_is_hidden("is_hidden"),
	s_MongoDriverServer_is_passive("is_passive"),
	s_MongoDriverServer_tags("tags"),
	s_MongoDriverServer_last_is_master("last_is_master"),
	s_MongoDriverServer_round_trip_time("round_trip_time");

Object hippo_mongo_driver_server_create_from_id(mongoc_client_t *client, uint32_t server_id)
{
	static Class* c_server;
	mongoc_server_description_t *sd;

	c_server = Unit::lookupClass(s_MongoDriverServer_className.get());
	Object tmp = Object{c_server};

	sd = mongoc_topology_description_server_by_id(&client->topology->description, server_id);

	if (!sd) {
		throw MongoDriver::Utils::CreateAndConstruct(
			MongoDriver::s_MongoDriverExceptionRuntimeException_className,
			HPHP::Variant("Failed to get server description, server likely gone"),
			HPHP::Variant((uint64_t) 0)
		);
	}

	tmp->o_set(s_MongoDriverServer___serverId, sd->host.host_and_port, s_MongoDriverServer_className);

	MongoDBDriverServerData* result_data = Native::data<MongoDBDriverServerData>(tmp);

	result_data->m_client = client;
	result_data->m_server_id = server_id;

	return tmp;
}


static bool mongodb_driver_add_server_debug(mongoc_server_description_t *sd, Array *retval)
{
	Variant v_last_is_master;
	Array   a_last_is_master;

	retval->set(s_MongoDriverServer_host, sd->host.host);
	retval->set(s_MongoDriverServer_port, sd->host.port);
	retval->set(s_MongoDriverServer_type, sd->type);
	retval->set(s_MongoDriverServer_is_primary, !!(sd->type == MONGOC_SERVER_RS_PRIMARY));
	retval->set(s_MongoDriverServer_is_secondary, !!(sd->type == MONGOC_SERVER_RS_SECONDARY));
	retval->set(s_MongoDriverServer_is_arbiter, !!(sd->type == MONGOC_SERVER_RS_ARBITER));

	hippo_bson_conversion_options_t options = HIPPO_TYPEMAP_DEBUG_INITIALIZER;
	BsonToVariantConverter convertor(bson_get_data(&sd->last_is_master), sd->last_is_master.len, options);
	convertor.convert(&v_last_is_master);
	a_last_is_master = v_last_is_master.toArray();

	retval->set(s_MongoDriverServer_is_hidden, a_last_is_master.exists(s_MongoDriverServer_hidden) && !!a_last_is_master[s_MongoDriverServer_hidden].toBoolean());
	retval->set(s_MongoDriverServer_is_passive, a_last_is_master.exists(s_MongoDriverServer_passive) && !!a_last_is_master[s_MongoDriverServer_passive].toBoolean());

	if (sd->tags.len) {
		Variant v_tags;

		hippo_bson_conversion_options_t options = HIPPO_TYPEMAP_DEBUG_INITIALIZER;
		BsonToVariantConverter convertor(bson_get_data(&sd->tags), sd->tags.len, options);
		convertor.convert(&v_tags);
		retval->set(s_MongoDriverServer_tags, v_tags);
	}

	retval->set(s_MongoDriverServer_last_is_master, a_last_is_master);

	retval->set(s_MongoDriverServer_round_trip_time, sd->round_trip_time);

	return true;
}

bool mongodb_driver_add_server_debug_wrapper(void *item, void *context)
{
	mongoc_server_description_t *server = (mongoc_server_description_t*) item;
	Array                       *retval = (Array*) context;
	Array                        server_info = Array::Create();
	bool                         ret;

	ret = mongodb_driver_add_server_debug(server, &server_info);
	retval->append(server_info);

	return ret;
}

Array HHVM_METHOD(MongoDBDriverServer, __debugInfo)
{
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);
	mongoc_server_description_t *sd;

	Array retval = Array::Create();

	if ((sd = mongoc_topology_description_server_by_id(&data->m_client->topology->description, data->m_server_id))) {
		mongodb_driver_add_server_debug(sd, &retval);
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

bool HHVM_METHOD(MongoDBDriverServer, isSecondary)
{
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);
	mongoc_server_description_t *sd;

	if ((sd = mongoc_topology_description_server_by_id(&data->m_client->topology->description, data->m_server_id))) {
		return (bool) (sd->type == MONGOC_SERVER_RS_SECONDARY);
	}

	throw MongoDriver::Utils::CreateAndConstruct(MongoDriver::s_MongoDriverExceptionRuntimeException_className, HPHP::Variant("Failed to get server description, server likely gone"), HPHP::Variant((uint64_t) 0));
}

bool HHVM_METHOD(MongoDBDriverServer, isArbiter)
{
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);
	mongoc_server_description_t *sd;

	if ((sd = mongoc_topology_description_server_by_id(&data->m_client->topology->description, data->m_server_id))) {
		return (bool) (sd->type == MONGOC_SERVER_RS_ARBITER);
	}

	throw MongoDriver::Utils::CreateAndConstruct(MongoDriver::s_MongoDriverExceptionRuntimeException_className, HPHP::Variant("Failed to get server description, server likely gone"), HPHP::Variant((uint64_t) 0));
}

bool HHVM_METHOD(MongoDBDriverServer, isHidden)
{
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);
	mongoc_server_description_t *sd;

	if ((sd = mongoc_topology_description_server_by_id(&data->m_client->topology->description, data->m_server_id))) {
		bson_iter_t iter;

		return !!(bson_iter_init_find_case(&iter, &sd->last_is_master, "hidden") && bson_iter_as_bool(&iter));
	}

	throw MongoDriver::Utils::CreateAndConstruct(MongoDriver::s_MongoDriverExceptionRuntimeException_className, HPHP::Variant("Failed to get server description, server likely gone"), HPHP::Variant((uint64_t) 0));
}

bool HHVM_METHOD(MongoDBDriverServer, isPassive)
{
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);
	mongoc_server_description_t *sd;

	if ((sd = mongoc_topology_description_server_by_id(&data->m_client->topology->description, data->m_server_id))) {
		bson_iter_t iter;

		return !!(bson_iter_init_find_case(&iter, &sd->last_is_master, "passive") && bson_iter_as_bool(&iter));
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
		readPreference
	);
}

Object HHVM_METHOD(MongoDBDriverServer, executeQuery, const String &ns, const Object &query, const Variant &readPreference)
{
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);

	return MongoDriver::Utils::doExecuteQuery(
		ns,
		data->m_client,
		data->m_server_id,
		query,
		readPreference
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
