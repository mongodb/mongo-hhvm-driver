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
	s_MongoDriverServer_round_trip_time("round_trip_time"),
	s_command("command");

hippo_server_description_type_map_t hippo_server_description_type_map[HIPPO_SERVER_DESCRIPTION_TYPES] = {
	{ HIPPO_SERVER_UNKNOWN, "Unknown" },
	{ HIPPO_SERVER_STANDALONE, "Standalone" },
	{ HIPPO_SERVER_MONGOS, "Mongos" },
	{ HIPPO_SERVER_POSSIBLE_PRIMARY, "PossiblePrimary" },
	{ HIPPO_SERVER_RS_PRIMARY, "RSPrimary" },
	{ HIPPO_SERVER_RS_SECONDARY, "RSSecondary" },
	{ HIPPO_SERVER_RS_ARBITER, "RSArbiter" },
	{ HIPPO_SERVER_RS_OTHER, "RSOther" },
	{ HIPPO_SERVER_RS_GHOST, "RSGhost" },
};

Object hippo_mongo_driver_server_create_from_id(mongoc_client_t *client, uint32_t server_id)
{
	static Class* c_server;
	mongoc_server_description_t *sd;
	bson_error_t error;

	c_server = Unit::lookupClass(s_MongoDriverServer_className.get());
	Object tmp = Object{c_server};

	sd = mongoc_topology_description_server_by_id(&client->topology->description, server_id, &error);

	if (!sd) {
		throw MongoDriver::Utils::CreateAndConstruct(
			MongoDriver::s_MongoDriverExceptionRuntimeException_className,
			"Failed to get server description, server likely gone: " + HPHP::Variant(error.message).toString(),
			HPHP::Variant((uint64_t) 0)
		);
	}

	tmp->o_set(s_MongoDriverServer___serverId, sd->host.host_and_port, s_MongoDriverServer_className);

	MongoDBDriverServerData* result_data = Native::data<MongoDBDriverServerData>(tmp);

	result_data->m_client = client;
	result_data->m_server_id = server_id;

	return tmp;
}

hippo_server_description_type_t hippo_server_description_type(mongoc_server_description_t *sd)
{
	int i;
	const char *name = mongoc_server_description_type(sd);

	for (i = 0; i < HIPPO_SERVER_DESCRIPTION_TYPES; i++) {
		if (strcmp(name, hippo_server_description_type_map[i].name) == 0) {
			return hippo_server_description_type_map[i].type;
		}
	}

	return HIPPO_SERVER_UNKNOWN;
}


static bool mongodb_driver_add_server_debug(mongoc_server_description_t *sd, Array *retval)
{
	mongoc_host_list_t *host = mongoc_server_description_host(sd);
	const bson_t       *is_master = mongoc_server_description_ismaster(sd);

	Variant v_last_is_master;
	Array   a_last_is_master;

	retval->set(s_MongoDriverServer_host, host->host);
	retval->set(s_MongoDriverServer_port, host->port);
	retval->set(s_MongoDriverServer_type, hippo_server_description_type(sd));
	retval->set(s_MongoDriverServer_is_primary, strcmp(mongoc_server_description_type(sd), hippo_server_description_type_map[HIPPO_SERVER_RS_PRIMARY].name) == 0);
	retval->set(s_MongoDriverServer_is_secondary, strcmp(mongoc_server_description_type(sd), hippo_server_description_type_map[HIPPO_SERVER_RS_SECONDARY].name) == 0);
	retval->set(s_MongoDriverServer_is_arbiter, strcmp(mongoc_server_description_type(sd), hippo_server_description_type_map[HIPPO_SERVER_RS_ARBITER].name) == 0);

	hippo_bson_conversion_options_t options = HIPPO_TYPEMAP_DEBUG_INITIALIZER;
	BsonToVariantConverter convertor(bson_get_data(is_master), is_master->len, options);
	convertor.convert(&v_last_is_master);
	a_last_is_master = v_last_is_master.toArray();

	retval->set(s_MongoDriverServer_is_hidden, a_last_is_master.exists(s_MongoDriverServer_hidden) && !!a_last_is_master[s_MongoDriverServer_hidden].toBoolean());
	retval->set(s_MongoDriverServer_is_passive, a_last_is_master.exists(s_MongoDriverServer_passive) && !!a_last_is_master[s_MongoDriverServer_passive].toBoolean());

	if (a_last_is_master.exists(s_MongoDriverServer_tags)) {
		retval->set(s_MongoDriverServer_tags, a_last_is_master[s_MongoDriverServer_tags]);
	}

	retval->set(s_MongoDriverServer_last_is_master, a_last_is_master);

	retval->set(s_MongoDriverServer_round_trip_time, mongoc_server_description_round_trip_time(sd));

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
	bson_error_t error;

	Array retval = Array::Create();

	if ((sd = mongoc_topology_description_server_by_id(&data->m_client->topology->description, data->m_server_id, &error))) {
		mongodb_driver_add_server_debug(sd, &retval);

		mongoc_server_description_destroy(sd);
		return retval;
	}

	throw MongoDriver::Utils::CreateAndConstruct(MongoDriver::s_MongoDriverExceptionRuntimeException_className, "Failed to get server description", HPHP::Variant((uint64_t) 0));
}

String HHVM_METHOD(MongoDBDriverServer, getHost)
{
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);
	mongoc_server_description_t *sd;

	if ((sd = mongoc_client_get_server_description(data->m_client, data->m_server_id))) {
		String host(mongoc_server_description_host(sd)->host);
		mongoc_server_description_destroy(sd);

		return host;
	}

	throw MongoDriver::Utils::CreateAndConstruct(MongoDriver::s_MongoDriverExceptionRuntimeException_className, "Failed to get server description", HPHP::Variant((uint64_t) 0));
}

Array HHVM_METHOD(MongoDBDriverServer, getInfo)
{
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);
	mongoc_server_description_t *sd;

	if ((sd = mongoc_client_get_server_description(data->m_client, data->m_server_id))) {
		const bson_t       *is_master = mongoc_server_description_ismaster(sd);

		Variant v;

		hippo_bson_conversion_options_t options = HIPPO_TYPEMAP_DEBUG_INITIALIZER;

		/* Yeah, this is not pretty. But, C++ doesn't have finally, and I don't
		 * want to bson_copy the is_master thing just because of that */
		try {
			BsonToVariantConverter convertor(bson_get_data(is_master), is_master->len, options);
			convertor.convert(&v);
		} catch (...) {
			mongoc_server_description_destroy(sd);
			throw;
		}

		return v.toArray();
	}

	throw MongoDriver::Utils::CreateAndConstruct(MongoDriver::s_MongoDriverExceptionRuntimeException_className, "Failed to get server description", HPHP::Variant((uint64_t) 0));
}

int64_t HHVM_METHOD(MongoDBDriverServer, getLatency)
{
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);
	mongoc_server_description_t *sd;

	if ((sd = mongoc_client_get_server_description(data->m_client, data->m_server_id))) {
		int64_t round_trip;

		round_trip = mongoc_server_description_round_trip_time(sd);

		mongoc_server_description_destroy(sd);

		return round_trip;
	}

	throw MongoDriver::Utils::CreateAndConstruct(MongoDriver::s_MongoDriverExceptionRuntimeException_className, "Failed to get server description", HPHP::Variant((uint64_t) 0));
}

int64_t HHVM_METHOD(MongoDBDriverServer, getPort)
{
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);
	mongoc_server_description_t *sd;

	if ((sd = mongoc_client_get_server_description(data->m_client, data->m_server_id))) {
		int64_t port;

		port = mongoc_server_description_host(sd)->port;
		mongoc_server_description_destroy(sd);

		return port;
	}

	throw MongoDriver::Utils::CreateAndConstruct(MongoDriver::s_MongoDriverExceptionRuntimeException_className, "Failed to get server description", HPHP::Variant((uint64_t) 0));
}

Array HHVM_METHOD(MongoDBDriverServer, getTags)
{
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);
	mongoc_server_description_t *sd;

	if ((sd = mongoc_client_get_server_description(data->m_client, data->m_server_id))) {
		const bson_t       *is_master = mongoc_server_description_ismaster(sd);

		Variant v_last_is_master;
		Array   a_last_is_master;

		hippo_bson_conversion_options_t options = HIPPO_TYPEMAP_DEBUG_INITIALIZER;

		/* Yeah, this is not pretty. But, C++ doesn't have finally, and I don't
		 * want to bson_copy the is_master thing just because of that */
		try {
			BsonToVariantConverter convertor(bson_get_data(is_master), is_master->len, options);
			convertor.convert(&v_last_is_master);
		} catch (...) {
			mongoc_server_description_destroy(sd);
			throw;
		}

		a_last_is_master = v_last_is_master.toArray();

		if (a_last_is_master.exists(s_MongoDriverServer_tags)) {
			return a_last_is_master[s_MongoDriverServer_tags].toArray();
		}

		mongoc_server_description_destroy(sd);
		a_last_is_master = Array();
		return a_last_is_master;
	}

	throw MongoDriver::Utils::CreateAndConstruct(MongoDriver::s_MongoDriverExceptionRuntimeException_className, "Failed to get server description", HPHP::Variant((uint64_t) 0));
}

int64_t HHVM_METHOD(MongoDBDriverServer, getType)
{
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);
	mongoc_server_description_t *sd;

	if ((sd = mongoc_client_get_server_description(data->m_client, data->m_server_id))) {
		int type;

		type = hippo_server_description_type(sd);

		mongoc_server_description_destroy(sd);

		return type;
	}

	throw MongoDriver::Utils::CreateAndConstruct(MongoDriver::s_MongoDriverExceptionRuntimeException_className, "Failed to get server description", HPHP::Variant((uint64_t) 0));
}

bool HHVM_METHOD(MongoDBDriverServer, isPrimary)
{
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);
	mongoc_server_description_t *sd;

	if ((sd = mongoc_client_get_server_description(data->m_client, data->m_server_id))) {
		bool isType;

		isType = (strcmp(mongoc_server_description_type(sd), hippo_server_description_type_map[HIPPO_SERVER_RS_PRIMARY].name) == 0);

		mongoc_server_description_destroy(sd);

		return isType;
	}

	throw MongoDriver::Utils::CreateAndConstruct(MongoDriver::s_MongoDriverExceptionRuntimeException_className, "Failed to get server description", HPHP::Variant((uint64_t) 0));
}

bool HHVM_METHOD(MongoDBDriverServer, isSecondary)
{
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);
	mongoc_server_description_t *sd;

	if ((sd = mongoc_client_get_server_description(data->m_client, data->m_server_id))) {
		bool isType;

		isType = (strcmp(mongoc_server_description_type(sd), hippo_server_description_type_map[HIPPO_SERVER_RS_SECONDARY].name) == 0);

		mongoc_server_description_destroy(sd);

		return isType;
	}

	throw MongoDriver::Utils::CreateAndConstruct(MongoDriver::s_MongoDriverExceptionRuntimeException_className, "Failed to get server description", HPHP::Variant((uint64_t) 0));
}

bool HHVM_METHOD(MongoDBDriverServer, isArbiter)
{
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);
	mongoc_server_description_t *sd;

	if ((sd = mongoc_client_get_server_description(data->m_client, data->m_server_id))) {
		bool isType;

		isType = (strcmp(mongoc_server_description_type(sd), hippo_server_description_type_map[HIPPO_SERVER_RS_ARBITER].name) == 0);

		mongoc_server_description_destroy(sd);

		return isType;
	}

	throw MongoDriver::Utils::CreateAndConstruct(MongoDriver::s_MongoDriverExceptionRuntimeException_className, "Failed to get server description", HPHP::Variant((uint64_t) 0));
}

bool HHVM_METHOD(MongoDBDriverServer, isHidden)
{
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);
	mongoc_server_description_t *sd;

	if ((sd = mongoc_client_get_server_description(data->m_client, data->m_server_id))) {
		bson_iter_t iter;

		const bson_t *is_master = mongoc_server_description_ismaster(sd);
		bool retval = !!(bson_iter_init_find_case(&iter, is_master, "hidden") && bson_iter_as_bool(&iter));

		mongoc_server_description_destroy(sd);

		return retval;
	}

	throw MongoDriver::Utils::CreateAndConstruct(MongoDriver::s_MongoDriverExceptionRuntimeException_className, "Failed to get server description", HPHP::Variant((uint64_t) 0));
}

bool HHVM_METHOD(MongoDBDriverServer, isPassive)
{
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);
	mongoc_server_description_t *sd;

	if ((sd = mongoc_client_get_server_description(data->m_client, data->m_server_id))) {
		bson_iter_t iter;

		const bson_t *is_master = mongoc_server_description_ismaster(sd);
		bool retval = !!(bson_iter_init_find_case(&iter, is_master, "passive") && bson_iter_as_bool(&iter));

		mongoc_server_description_destroy(sd);

		return retval;
	}

	throw MongoDriver::Utils::CreateAndConstruct(MongoDriver::s_MongoDriverExceptionRuntimeException_className, "Failed to get server description", HPHP::Variant((uint64_t) 0));
}

Object HHVM_METHOD(MongoDBDriverServer, executeCommand, const String &db, const Object &command, const Variant &readPreference)
{
	MongoDBDriverServerData* data = Native::data<MongoDBDriverServerData>(this_);

	return MongoDriver::Utils::doExecuteCommand(
		db.c_str(),
		data->m_client,
		data->m_server_id,
		command,
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
