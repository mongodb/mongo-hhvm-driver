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
#include "hphp/runtime/base/array-iterator.h"
#include "hphp/runtime/ext/stream/ext_stream.h"

#undef TRACE

#include "../../../bson.h"
#include "../../../pool.h"
#include "../../../utils.h"
#include "../../../mongodb.h"

extern "C" {
#include "../../../libmongoc/src/mongoc/mongoc-client.h"
}

#include "BulkWrite.h"
#include "Command.h"
#include "Cursor.h"
#include "Manager.h"
#include "Query.h"
#include "ReadConcern.h"
#include "ReadPreference.h"
#include "Server.h"
#include "WriteConcern.h"
#include "WriteResult.h"

namespace HPHP {

Class* MongoDBDriverManagerData::s_class = nullptr;
const StaticString MongoDBDriverManagerData::s_className("MongoDBDriverManager");
IMPLEMENT_GET_CLASS(MongoDBDriverManagerData);

const StaticString
	s_MongoDBDriverManager_slaveok("slaveok"),
	s_MongoDBDriverManager_readpreference("readpreference"),
	s_MongoDBDriverManager_readpreferencetags("readpreferencetags"),
	s_MongoDBDriverManager_maxstalenessms("maxstalenessms"),
	s_MongoDBDriverManager_readPreference("readPreference"),
	s_MongoDBDriverManager_readPreferenceTags("readPreferenceTags"),
	s_MongoDBDriverManager_maxStalenessMS("maxStalenessMS"),
	s_MongoDBDriverManager_readconcernlevel("readconcernlevel"),
	s_MongoDBDriverManager_readConcernLevel("readConcernLevel"),
	s_MongoDBDriverManager_mode("mode"),
	s_MongoDBDriverManager_tags("tags"),
	s_MongoDBDriverManager_w("w"),
	s_MongoDBDriverManager_wmajority("wmajority"),
	s_MongoDBDriverManager_wtimeout("wtimeout"),
	s_MongoDBDriverManager_wtimeoutms("wtimeoutms"),
	s_MongoDBDriverManager_safe("safe"),
	s_MongoDBDriverManager_journal("journal"),
	s_MongoDBDriverManager_context("context"),
	s_MongoDBDriverManager_appname("appname"),
	s_MongoDBDriverManager_context_ssl("ssl"),
	s_MongoDBDriverManager_context_ssl_allow_self_signed("allow_self_signed"),
	s_MongoDBDriverManager_context_ssl_local_cert("local_cert"),
	s_MongoDBDriverManager_context_ssl_passphrase("passphrase"),
	s_MongoDBDriverManager_context_ssl_cafile("cafile"),
	s_MongoDBDriverManager_context_ssl_capath("capath");

static bool hippo_mongo_driver_manager_apply_rc(mongoc_uri_t *uri, const Array options)
{
	mongoc_read_concern_t *new_rc;
	const mongoc_read_concern_t *old_rc;
	const char *rc_str = NULL;

	if (!(old_rc = mongoc_uri_get_read_concern(uri))) {
		throw MongoDriver::Utils::throwRunTimeException("mongoc_uri_t does not have a read concern");

		return false;
	}

	if (options.size() == 0) {
		return true;
	}

	if (
		!options.exists(s_MongoDBDriverManager_readConcernLevel) &&
		!options.exists(s_MongoDBDriverManager_readconcernlevel)
	) {
		return true;
	}

	new_rc = mongoc_read_concern_copy(old_rc);

	if (options.exists(s_MongoDBDriverManager_readconcernlevel) && options[s_MongoDBDriverManager_readconcernlevel].isString()) {
		rc_str = options[s_MongoDBDriverManager_readconcernlevel].toString().c_str();
	}
	if (options.exists(s_MongoDBDriverManager_readConcernLevel) && options[s_MongoDBDriverManager_readConcernLevel].isString()) {
		rc_str = options[s_MongoDBDriverManager_readConcernLevel].toString().c_str();
	}

	if (rc_str) {
		mongoc_read_concern_set_level(new_rc, rc_str);
	}

	mongoc_uri_set_read_concern(uri, new_rc);
	mongoc_read_concern_destroy(new_rc);

	return true;
}

static bool hippo_mongo_driver_manager_apply_rp(mongoc_uri_t *uri, const Array options)
{
	mongoc_read_prefs_t *new_rp;
	const mongoc_read_prefs_t *old_rp;
	const char *rp_str = NULL;
	bson_t *b_tags;

	if (!(old_rp = mongoc_uri_get_read_prefs_t(uri))) {
		throw MongoDriver::Utils::throwRunTimeException("mongoc_uri_t does not have a read preference");

		return false;
	}

	if (options.size() == 0) {
		return true;
	}

	if (
		!options.exists(s_MongoDBDriverManager_slaveok) &&
		!options.exists(s_MongoDBDriverManager_readpreference) &&
		!options.exists(s_MongoDBDriverManager_readpreferencetags) &&
		!options.exists(s_MongoDBDriverManager_maxstalenessms) &&
		!options.exists(s_MongoDBDriverManager_readPreference) &&
		!options.exists(s_MongoDBDriverManager_readPreferenceTags) &&
		!options.exists(s_MongoDBDriverManager_maxStalenessMS)
	) {
		return true;
	}

	new_rp = mongoc_read_prefs_copy(old_rp);

	if (options.exists(s_MongoDBDriverManager_slaveok) && options[s_MongoDBDriverManager_slaveok].isBoolean()) {
		mongoc_read_prefs_set_mode(new_rp, MONGOC_READ_SECONDARY_PREFERRED);
	}

	if (options.exists(s_MongoDBDriverManager_readpreference) && options[s_MongoDBDriverManager_readpreference].isString()) {
		rp_str = options[s_MongoDBDriverManager_readpreference].toString().c_str();
	}
	if (options.exists(s_MongoDBDriverManager_readPreference) && options[s_MongoDBDriverManager_readPreference].isString()) {
		rp_str = options[s_MongoDBDriverManager_readPreference].toString().c_str();
	}

	if (rp_str) {
		if (0 == strcasecmp("primary", rp_str)) {
			mongoc_read_prefs_set_mode(new_rp, MONGOC_READ_PRIMARY);
		} else if (0 == strcasecmp("primarypreferred", rp_str)) {
			mongoc_read_prefs_set_mode(new_rp, MONGOC_READ_PRIMARY_PREFERRED);
		} else if (0 == strcasecmp("secondary", rp_str)) {
			mongoc_read_prefs_set_mode(new_rp, MONGOC_READ_SECONDARY);
		} else if (0 == strcasecmp("secondarypreferred", rp_str)) {
			mongoc_read_prefs_set_mode(new_rp, MONGOC_READ_SECONDARY_PREFERRED);
		} else if (0 == strcasecmp("nearest", rp_str)) {
			mongoc_read_prefs_set_mode(new_rp, MONGOC_READ_NEAREST);
		} else {
			throw MongoDriver::Utils::throwInvalidArgumentException("Unsupported readPreference value: '" + Variant(rp_str).toString() + "'");
			mongoc_read_prefs_destroy(new_rp);

			return false;
		}
	}

	if (options.exists(s_MongoDBDriverManager_readpreferencetags) && options[s_MongoDBDriverManager_readpreferencetags].isArray()) {
		VariantToBsonConverter converter(options[s_MongoDBDriverManager_readpreferencetags].toArray(), HIPPO_BSON_NO_FLAGS);
		b_tags = bson_new();
		converter.convert(b_tags);
		mongoc_read_prefs_set_tags(new_rp, b_tags);
	} else if (options.exists(s_MongoDBDriverManager_readPreferenceTags) && options[s_MongoDBDriverManager_readPreferenceTags].isArray()) {
		VariantToBsonConverter converter(options[s_MongoDBDriverManager_readPreferenceTags].toArray(), HIPPO_BSON_NO_FLAGS);
		b_tags = bson_new();
		converter.convert(b_tags);
		mongoc_read_prefs_set_tags(new_rp, b_tags);
	}

	/* Validate that readPreferenceTags are not used with PRIMARY readPreference */
	if (
		mongoc_read_prefs_get_mode(new_rp) == MONGOC_READ_PRIMARY &&
		!bson_empty(mongoc_read_prefs_get_tags(new_rp))
	) {
		throw MongoDriver::Utils::throwInvalidArgumentException("Primary read preference mode conflicts with tags");
		mongoc_read_prefs_destroy(new_rp);
		return false;
	}

	/* Handle maxStalenessMS, and make sure it is not combined with PRIMARY readPreference */
	if (
		(options.exists(s_MongoDBDriverManager_maxstalenessms) && options[s_MongoDBDriverManager_maxstalenessms].isInteger())
		||
		(options.exists(s_MongoDBDriverManager_maxStalenessMS) && options[s_MongoDBDriverManager_maxStalenessMS].isInteger())
	) {
		if (mongoc_read_prefs_get_mode(new_rp) == MONGOC_READ_PRIMARY) {
			throw MongoDriver::Utils::throwInvalidArgumentException("Primary read preference mode conflicts with maxStalenessMS");
			mongoc_read_prefs_destroy(new_rp);

			return false;
		}

		if (options.exists(s_MongoDBDriverManager_maxstalenessms)) {
			mongoc_read_prefs_set_max_staleness_ms(new_rp, (int32_t) options[s_MongoDBDriverManager_maxstalenessms].toInt64());
		} else {
			mongoc_read_prefs_set_max_staleness_ms(new_rp, (int32_t) options[s_MongoDBDriverManager_maxStalenessMS].toInt64());
		}
	}

	/* This may be redundant in light of the check for primary with tags,
	 * but we'll check anyway in case additional validation is implemented. */
	if (!mongoc_read_prefs_is_valid(new_rp)) {
		throw MongoDriver::Utils::throwInvalidArgumentException("Read preference is not valid");
		mongoc_read_prefs_destroy(new_rp);

		return false;
	}

	mongoc_uri_set_read_prefs_t(uri, new_rp);
	mongoc_read_prefs_destroy(new_rp);

	return true;
}

static bool hippo_mongo_driver_manager_apply_wc(mongoc_uri_t *uri, const Array options)
{
	int32_t wtimeoutms;
	mongoc_write_concern_t *new_wc;
	const mongoc_write_concern_t *old_wc;

	if (!(old_wc = mongoc_uri_get_write_concern(uri))) {
		throw MongoDriver::Utils::throwRunTimeException("mongoc_uri_t does not have a write concern");

		return false;
	}

	if (options.size() == 0) {
		return true;
	}

	if (
		!options.exists(s_MongoDBDriverManager_journal) &&
		!options.exists(s_MongoDBDriverManager_safe) &&
		!options.exists(s_MongoDBDriverManager_w) &&
		!options.exists(s_MongoDBDriverManager_wtimeoutms)
	) {
		return true;
	}

	wtimeoutms = mongoc_write_concern_get_wtimeout(old_wc);

	new_wc = mongoc_write_concern_copy(old_wc);

	if (options.exists(s_MongoDBDriverManager_safe) && options[s_MongoDBDriverManager_safe].isBoolean()) {
		mongoc_write_concern_set_w(new_wc, options[s_MongoDBDriverManager_safe].toBoolean() ? 1 : MONGOC_WRITE_CONCERN_W_UNACKNOWLEDGED);
	}

	if (options.exists(s_MongoDBDriverManager_wtimeoutms) && options[s_MongoDBDriverManager_wtimeoutms].isInteger()) {
		wtimeoutms = (int32_t) options[s_MongoDBDriverManager_wtimeoutms].toInt64();
	}

	if (options.exists(s_MongoDBDriverManager_journal) && options[s_MongoDBDriverManager_journal].isBoolean()) {
		mongoc_write_concern_set_journal(new_wc, !!options[s_MongoDBDriverManager_journal].toBoolean());
	}

	if (options.exists(s_MongoDBDriverManager_w)) {
		if (options[s_MongoDBDriverManager_w].isInteger()) {
			int32_t value = (int32_t) options[s_MongoDBDriverManager_w].toInt64();

			switch (value) {
				case MONGOC_WRITE_CONCERN_W_ERRORS_IGNORED:
				case MONGOC_WRITE_CONCERN_W_UNACKNOWLEDGED:
					mongoc_write_concern_set_w(new_wc, value);
					break;

				default:
					if (value > 0) {
						mongoc_write_concern_set_w(new_wc, value);
						break;
					}
					throw MongoDriver::Utils::throwInvalidArgumentException("Unsupported w value: " + Variant(value).toString());
					mongoc_write_concern_destroy(new_wc);

					return false;
			}
		} else if (options[s_MongoDBDriverManager_w].isString()) {
			const char *str = options[s_MongoDBDriverManager_w].toString().c_str();

			if (0 == strcasecmp("majority", str)) {
				mongoc_write_concern_set_wmajority(new_wc, wtimeoutms);
			} else {
				mongoc_write_concern_set_wtag(new_wc, str);
			}
		}
	}

	/* Only set wtimeout if it's still applicable; otherwise, clear it. */
	if (mongoc_write_concern_get_w(new_wc) > 1 ||
		mongoc_write_concern_get_wmajority(new_wc) ||
		mongoc_write_concern_get_wtag(new_wc)) {
		mongoc_write_concern_set_wtimeout(new_wc, wtimeoutms);
	} else {
		mongoc_write_concern_set_wtimeout(new_wc, 0);
	}

	if (mongoc_write_concern_get_journal(new_wc)) {
		int32_t w = mongoc_write_concern_get_w(new_wc);

		if (w == MONGOC_WRITE_CONCERN_W_UNACKNOWLEDGED || w == MONGOC_WRITE_CONCERN_W_ERRORS_IGNORED) {
			throw MongoDriver::Utils::throwInvalidArgumentException("Journal conflicts with w value: " + Variant(w).toString());
			mongoc_write_concern_destroy(new_wc);

			return false;
		}
	}

	/* This may be redundant in light of the last check (unacknowledged w with
	 * journal), but we'll check anyway in case additional validation is
	 * implemented. */
	if (!mongoc_write_concern_is_valid(new_wc)) {
		throw MongoDriver::Utils::throwInvalidArgumentException("Write concern is not valid");
		mongoc_write_concern_destroy(new_wc);

		return false;
	}

	mongoc_uri_set_write_concern(uri, new_wc);
	mongoc_write_concern_destroy(new_wc);

	return true;
}

static mongoc_uri_t *hippo_mongo_driver_manager_make_uri(const char *dsn, const Array options)
{
	mongoc_uri_t *uri = mongoc_uri_new(dsn);

	if (!uri) {
		throw MongoDriver::Utils::throwInvalidArgumentException("Failed to parse MongoDB URI: '" + String(dsn) + "'");
	}

	for (ArrayIter iter(options); iter; ++iter) {
		const Variant& key = iter.first();
		const Variant& value = iter.second();
		const char *s_key = key.toString().c_str();

		if (
			!strcasecmp(s_key, "journal") ||
			!strcasecmp(s_key, "readpreference") ||
			!strcasecmp(s_key, "readpreferencetags") ||
			!strcasecmp(s_key, "safe") ||
			!strcasecmp(s_key, "slaveok") ||
			!strcasecmp(s_key, "w") ||
			!strcasecmp(s_key, "wtimeoutms") ||
			!strcasecmp(s_key, "maxstalenessms") ||
			!strcasecmp(s_key, "appname")
		) {
			continue;
		}

		if (mongoc_uri_option_is_bool(s_key)) {
			mongoc_uri_set_option_as_bool(uri, s_key, value.toBoolean());
		} else if (mongoc_uri_option_is_int32(s_key) && value.isInteger()) {
			mongoc_uri_set_option_as_int32(uri, s_key, (int32_t) value.toInt64());
		} else if (mongoc_uri_option_is_utf8(s_key) && value.isString()) {
			mongoc_uri_set_option_as_utf8(uri, s_key, value.toString().c_str());
		} else if (value.isString()) {
			if (!strcasecmp(s_key, "username")) {
				mongoc_uri_set_username(uri, value.toString().c_str());
			} else if (!strcasecmp(s_key, "password")) {
				mongoc_uri_set_password(uri, value.toString().c_str());
			} else if (!strcasecmp(s_key, "database")) {
				mongoc_uri_set_database(uri, value.toString().c_str());
			} else if (!strcasecmp(s_key, "authsource")) {
				mongoc_uri_set_auth_source(uri, value.toString().c_str());
			}
		}
	}

	return uri;
}

static bool hippo_mongo_driver_manager_apply_ssl_opts(mongoc_client_t *client, const Array options)
{
	mongoc_ssl_opt_t ssl_opt = { NULL, NULL, NULL, NULL, NULL, true };
	Array ssl;
	bool apply_ssl = false;

	if (!mongoc_uri_get_ssl(mongoc_client_get_uri(client))) {
		return 0;
	}

	if (options.exists(s_MongoDBDriverManager_context)) {
		Array context;

		if (!options[s_MongoDBDriverManager_context].isResource()) {
			return 0;
		}

#if HIPPO_HHVM_VERSION >= 30900
		req::ptr<StreamContext> sc;
#else
		SmartPtr<StreamContext> sc;
#endif
		sc = cast<StreamContext>(options[s_MongoDBDriverManager_context]);
		context = sc->getOptions();

		if (!context.exists(s_MongoDBDriverManager_context_ssl)) {
			return 0;
		}

		if (!context[s_MongoDBDriverManager_context_ssl].isArray()) {
			return 0;
		}

		ssl = context[s_MongoDBDriverManager_context_ssl].toArray();
	} else {
		ssl = options;
	}

	if (ssl.exists(s_MongoDBDriverManager_context_ssl_allow_self_signed) && ssl[s_MongoDBDriverManager_context_ssl_allow_self_signed].isBoolean()) {
		apply_ssl = true;
		ssl_opt.weak_cert_validation = (bool)true;//ssl[s_MongoDBDriverManager_context_ssl_allow_self_signed].toBoolean();
	}
	if (ssl.exists(s_MongoDBDriverManager_context_ssl_local_cert) && ssl[s_MongoDBDriverManager_context_ssl_local_cert].isString()) {
		apply_ssl = true;
		ssl_opt.pem_file = ssl[s_MongoDBDriverManager_context_ssl_local_cert].toString().c_str();
	}
	if (ssl.exists(s_MongoDBDriverManager_context_ssl_passphrase) && ssl[s_MongoDBDriverManager_context_ssl_passphrase].isString()) {
		apply_ssl = true;
		ssl_opt.pem_pwd = ssl[s_MongoDBDriverManager_context_ssl_passphrase].toString().c_str();
	}
	if (ssl.exists(s_MongoDBDriverManager_context_ssl_cafile) && ssl[s_MongoDBDriverManager_context_ssl_cafile].isString()) {
		apply_ssl = true;
		ssl_opt.ca_file = ssl[s_MongoDBDriverManager_context_ssl_cafile].toString().c_str();
	}
	if (ssl.exists(s_MongoDBDriverManager_context_ssl_capath) && ssl[s_MongoDBDriverManager_context_ssl_capath].isString()) {
		apply_ssl = true;
		ssl_opt.ca_dir = ssl[s_MongoDBDriverManager_context_ssl_capath].toString().c_str();
	}

	if (apply_ssl) {
		mongoc_client_set_ssl_opts(client, &ssl_opt);
	}
	return 1;
}


void MongoDBDriverManagerData::sweep()
{
	if (m_client) {
		Pool::ReturnClient(m_hash, m_client);
	}
}

void HHVM_METHOD(MongoDBDriverManager, __construct, const String &dsn, const Array &options, const Array &driverOptions)
{
	MongoDBDriverManagerData* data = Native::data<MongoDBDriverManagerData>(this_);
	mongoc_uri_t *uri;
	mongoc_client_t *client;

	uri = hippo_mongo_driver_manager_make_uri(dsn.c_str(), options);

	data->m_hash = Pool::CreateHash(uri, options, driverOptions);

	hippo_mongo_driver_manager_apply_rc(uri, options);
	hippo_mongo_driver_manager_apply_rp(uri, options);
	hippo_mongo_driver_manager_apply_wc(uri, options);

	if (options.exists(s_MongoDBDriverManager_appname)) {
		if (!mongoc_uri_set_appname(uri, options[s_MongoDBDriverManager_appname].toString().c_str())) {
			throw MongoDriver::Utils::throwInvalidArgumentException("Invalid appname value: '" + options[s_MongoDBDriverManager_appname].toString() + "'");
		}
	}

	client = Pool::GetClient(data->m_hash, uri);

	if (!client) {
		throw MongoDriver::Utils::throwRunTimeException("Failed to create Manager from URI: '" + dsn + "'");
	}

	data->m_client = client;

	hippo_mongo_driver_manager_apply_ssl_opts(data->m_client, driverOptions);
}

const StaticString
	s_MongoDBDriverManager_uri("uri"),
	s_MongoDBDriverManager_cluster("cluster"),
	s_MongoDBDriverManager_limit("limit"),
	s_MongoDBDriverManager_multi("multi"),
	s_MongoDBDriverManager_upsert("upsert"),
	s_MongoDBDriverManager_command("command");

Array HHVM_METHOD(MongoDBDriverManager, __debugInfo)
{
	MongoDBDriverManagerData     *data = Native::data<MongoDBDriverManagerData>(this_);
	size_t                        i;
	mongoc_server_description_t **sds;
	size_t                        n;
	Array retval = Array::Create();
	Array servers = Array::Create();

	retval.add(s_MongoDBDriverManager_uri, (char*) mongoc_uri_get_string(mongoc_client_get_uri(data->m_client)));

	sds = mongoc_client_get_server_descriptions(data->m_client, &n);
	for (i = 0; i < n; i++) {
		if (sds[i]->type == MONGOC_SERVER_UNKNOWN) {
			continue;
		}

		mongodb_driver_add_server_debug_wrapper(sds[i], &servers);
	}
	mongoc_server_descriptions_destroy_all(sds, n);

	retval.add(s_MongoDBDriverManager_cluster, servers);

	return retval;
}

void HHVM_METHOD(MongoDBDriverManager, __wakeup)
{
	throw MongoDriver::Utils::throwRunTimeException("Unserialization of MongoDB\\Driver\\Manager is not allowed");
}

Object HHVM_METHOD(MongoDBDriverManager, executeCommand, const String &db, const Object &command, const Variant &readPreference)
{
	MongoDBDriverManagerData* data = Native::data<MongoDBDriverManagerData>(this_);

	return MongoDriver::Utils::doExecuteCommand(
		db.c_str(),
		data->m_client,
		-1,
		command,
		readPreference
	);
}

Object HHVM_METHOD(MongoDBDriverManager, executeQuery, const String &ns, const Object &query, const Variant &readPreference)
{
	MongoDBDriverManagerData* manager_data = Native::data<MongoDBDriverManagerData>(this_);

	return MongoDriver::Utils::doExecuteQuery(
		ns,
		manager_data->m_client,
		-1,
		query,
		readPreference
	);
}

Object HHVM_METHOD(MongoDBDriverManager, executeBulkWrite, const String &ns, const Object &bulk, const Variant &writeConcern)
{
	const mongoc_write_concern_t *write_concern = NULL;
	MongoDBDriverManagerData* manager_data = Native::data<MongoDBDriverManagerData>(this_);

	/* Deal with write concerns */
	if (!writeConcern.isNull()) {
		HPHP::MongoDBDriverWriteConcernData* wc_data = HPHP::Native::data<HPHP::MongoDBDriverWriteConcernData>(writeConcern.toObject().get());
		write_concern = wc_data->m_write_concern;
	}

	return MongoDriver::Utils::doExecuteBulkWrite(
		ns,
		manager_data->m_client,
		-1,
		bulk,
		write_concern
	);
}

Object HHVM_METHOD(MongoDBDriverManager, getReadConcern)
{
	MongoDBDriverManagerData *data = Native::data<MongoDBDriverManagerData>(this_);

	Class *c_rc = Unit::lookupClass(s_MongoDriverReadConcern_className.get());
	assert(c_rc);
	Object rc_obj = Object{c_rc};
	MongoDBDriverReadConcernData* rc_data = Native::data<HPHP::MongoDBDriverReadConcernData>(rc_obj.get());

	rc_data->m_read_concern = mongoc_read_concern_copy(mongoc_client_get_read_concern(data->m_client));

	return rc_obj;
}

Object HHVM_METHOD(MongoDBDriverManager, getReadPreference)
{
	MongoDBDriverManagerData *data = Native::data<MongoDBDriverManagerData>(this_);

	Class *c_rp = Unit::lookupClass(s_MongoDriverReadPreference_className.get());
	assert(c_rp);
	Object rp_obj = Object{c_rp};
	MongoDBDriverReadPreferenceData* rp_data = Native::data<HPHP::MongoDBDriverReadPreferenceData>(rp_obj.get());

	rp_data->m_read_preference = mongoc_read_prefs_copy(mongoc_client_get_read_prefs(data->m_client));

	return rp_obj;
}

Array HHVM_METHOD(MongoDBDriverManager, getServers)
{
	MongoDBDriverManagerData     *data = Native::data<MongoDBDriverManagerData>(this_);
	size_t                        i;
	mongoc_server_description_t **sds;
	size_t                        n;

	Array retval = Array::Create();

	sds = mongoc_client_get_server_descriptions(data->m_client, &n);
	for (i = 0; i < n; i++) {
		if (sds[i]->type == MONGOC_SERVER_UNKNOWN) {
			continue;
		}

		retval.add(
			(int64_t) i,
			hippo_mongo_driver_server_create_from_id(data->m_client, sds[i]->id)
		);

	}
	mongoc_server_descriptions_destroy_all(sds, n);

	return retval;
}

Object HHVM_METHOD(MongoDBDriverManager, getWriteConcern)
{
	MongoDBDriverManagerData* data = Native::data<MongoDBDriverManagerData>(this_);

	Class *c_wc = Unit::lookupClass(s_MongoDriverWriteConcern_className.get());
	assert(c_wc);
	Object wc_obj = Object{c_wc};
	MongoDBDriverWriteConcernData* wc_data = Native::data<HPHP::MongoDBDriverWriteConcernData>(wc_obj.get());

	wc_data->m_write_concern = mongoc_write_concern_copy(mongoc_client_get_write_concern(data->m_client));

	return wc_obj;
}

Object HHVM_METHOD(MongoDBDriverManager, selectServer, const Object &readPreference)
{
	MongoDBDriverManagerData* data = Native::data<MongoDBDriverManagerData>(this_);
	MongoDBDriverReadPreferenceData *rp_data = Native::data<MongoDBDriverReadPreferenceData>(readPreference.get());
	bson_error_t error;
	mongoc_server_description_t *selected_server = NULL;
	Object tmp;

	selected_server = mongoc_client_select_server(data->m_client, false, rp_data->m_read_preference, &error);
	if (selected_server) {
		tmp = hippo_mongo_driver_server_create_from_id(data->m_client, mongoc_server_description_id(selected_server));
		mongoc_server_description_destroy(selected_server);
		return tmp;
	} else {
		throw MongoDriver::Utils::throwExceptionFromBsonError(&error);
	}
}

}
