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
#include "hphp/runtime/base/execution-context.h"
#include "hphp/runtime/ext/stream/ext_stream.h"

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
#include "Monitoring/_CommandEvent.h"
#include "Monitoring/_CommandResultEvent.h"
#include "Monitoring/CommandStartedEvent.h"
#include "Monitoring/CommandSucceededEvent.h"
#include "Monitoring/CommandFailedEvent.h"

namespace HPHP {

const StaticString s_MongoDriverManager_className("MongoDB\\Driver\\Manager");
Class* MongoDBDriverManagerData::s_class = nullptr;
const StaticString MongoDBDriverManagerData::s_className("MongoDBDriverManager");
IMPLEMENT_GET_CLASS(MongoDBDriverManagerData);

const StaticString
	s_MongoDBDriverManager_slaveok("slaveok"),
	s_MongoDBDriverManager_readpreference("readpreference"),
	s_MongoDBDriverManager_readpreferencetags("readpreferencetags"),
	s_MongoDBDriverManager_maxstalenessseconds("maxstalenessseconds"),
	s_MongoDBDriverManager_readPreference("readPreference"),
	s_MongoDBDriverManager_readPreferenceTags("readPreferenceTags"),
	s_MongoDBDriverManager_maxStalenessSeconds("maxStalenessSeconds"),
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
	s_MongoDBDriverManager_context_ssl_allow_invalid_hostname("allow_invalid_hostname"),
	s_MongoDBDriverManager_context_ssl_weak_cert_validation("weak_cert_validation"),
	s_MongoDBDriverManager_context_ssl_pem_file("pem_file"),
	s_MongoDBDriverManager_context_ssl_pem_pwd("pem_pwd"),
	s_MongoDBDriverManager_context_ssl_ca_file("ca_file"),
	s_MongoDBDriverManager_context_ssl_ca_dir("ca_dir"),
	s_MongoDBDriverManager_context_ssl_crl_file("crl_file"),
	/* Deprecated names */
	s_MongoDBDriverManager_context_ssl_allow_self_signed("allow_self_signed"),
	s_MongoDBDriverManager_context_ssl_local_cert("local_cert"),
	s_MongoDBDriverManager_context_ssl_passphrase("passphrase"),
	s_MongoDBDriverManager_context_ssl_cafile("cafile"),
	s_MongoDBDriverManager_context_ssl_capath("capath"),
	/* APM */
	s_MongoDBDriverManager_subscribers("subscribers"),
	s_MongoDBDriverManager_dispatchCommandStarted("_dispatchCommandStarted"),
	s_MongoDBDriverManager_dispatchCommandSucceeded("_dispatchCommandSucceeded"),
	s_MongoDBDriverManager_dispatchCommandFailed("_dispatchCommandFailed");

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
		!options.exists(s_MongoDBDriverManager_maxstalenessseconds) &&
		!options.exists(s_MongoDBDriverManager_readPreference) &&
		!options.exists(s_MongoDBDriverManager_readPreferenceTags) &&
		!options.exists(s_MongoDBDriverManager_maxStalenessSeconds)
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

	/* Validate that readPreferenceTags are not used with PRIMARY readPreference */
	if (
		mongoc_read_prefs_get_mode(new_rp) == MONGOC_READ_PRIMARY &&
		(
			options.exists(s_MongoDBDriverManager_readpreferencetags) ||
			options.exists(s_MongoDBDriverManager_readPreferenceTags)
		)
	) {
		throw MongoDriver::Utils::throwInvalidArgumentException("Primary read preference mode conflicts with tags");
		mongoc_read_prefs_destroy(new_rp);
		return false;
	}

	if (options.exists(s_MongoDBDriverManager_readpreferencetags) && options[s_MongoDBDriverManager_readpreferencetags].isArray()) {
		const Array a_tags = hippo_mongo_driver_readpreference_prep_tagsets(options[s_MongoDBDriverManager_readpreferencetags].toArray());

		if (!hippo_mongo_driver_readpreference_are_valid(a_tags)) {
			throw MongoDriver::Utils::throwInvalidArgumentException("Read preference tags must be an array of zero or more documents");
			mongoc_read_prefs_destroy(new_rp);
			return false;
		}

		VariantToBsonConverter converter(a_tags, HIPPO_BSON_NO_FLAGS);
		b_tags = bson_new();
		converter.convert(b_tags);
		mongoc_read_prefs_set_tags(new_rp, b_tags);
	} else if (options.exists(s_MongoDBDriverManager_readPreferenceTags) && options[s_MongoDBDriverManager_readPreferenceTags].isArray()) {
		const Array a_tags = hippo_mongo_driver_readpreference_prep_tagsets(options[s_MongoDBDriverManager_readPreferenceTags].toArray());

		if (!hippo_mongo_driver_readpreference_are_valid(a_tags)) {
			throw MongoDriver::Utils::throwInvalidArgumentException("Read preference tags must be an array of zero or more documents");
			mongoc_read_prefs_destroy(new_rp);
			return false;
		}

		VariantToBsonConverter converter(a_tags, HIPPO_BSON_NO_FLAGS);
		b_tags = bson_new();
		converter.convert(b_tags);
		mongoc_read_prefs_set_tags(new_rp, b_tags);
	}

	/* Handle maxStalenessSeconds, and make sure it is not combined with PRIMARY readPreference */
	if (
		(options.exists(s_MongoDBDriverManager_maxstalenessseconds) && options[s_MongoDBDriverManager_maxstalenessseconds].isInteger())
		||
		(options.exists(s_MongoDBDriverManager_maxStalenessSeconds) && options[s_MongoDBDriverManager_maxStalenessSeconds].isInteger())
	) {
		int64_t maxStalenessSeconds = 0;

		if (options.exists(s_MongoDBDriverManager_maxstalenessseconds)) {
			maxStalenessSeconds = options[s_MongoDBDriverManager_maxstalenessseconds].toInt64();
		} else {
			maxStalenessSeconds = options[s_MongoDBDriverManager_maxStalenessSeconds].toInt64();
		}

		if (maxStalenessSeconds != MONGOC_NO_MAX_STALENESS) {
			if (maxStalenessSeconds < MONGOC_SMALLEST_MAX_STALENESS_SECONDS) {
				throw MongoDriver::Utils::throwInvalidArgumentException("Expected maxStalenessSeconds to be >= " + Variant(MONGOC_SMALLEST_MAX_STALENESS_SECONDS).toString() + ", " + Variant(maxStalenessSeconds).toString() + " given");
			}
			if (maxStalenessSeconds >= 2147483647) {
				throw MongoDriver::Utils::throwInvalidArgumentException("Expected maxStalenessSeconds to be <= 2147483647, " + Variant(maxStalenessSeconds).toString() + " given");
			}

			if (mongoc_read_prefs_get_mode(new_rp) == MONGOC_READ_PRIMARY) {
				throw MongoDriver::Utils::throwInvalidArgumentException("Primary read preference mode conflicts with maxStalenessSeconds");
			}
		}

		mongoc_read_prefs_set_max_staleness_seconds(new_rp, (int32_t) maxStalenessSeconds);
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
			!strcasecmp(s_key, "maxstalenessseconds") ||
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

	if (ssl.exists(s_MongoDBDriverManager_context_ssl_allow_invalid_hostname) && ssl[s_MongoDBDriverManager_context_ssl_allow_invalid_hostname].isBoolean()) {
		apply_ssl = true;
		ssl_opt.allow_invalid_hostname = ssl[s_MongoDBDriverManager_context_ssl_allow_invalid_hostname].toBoolean();
	}

	if (ssl.exists(s_MongoDBDriverManager_context_ssl_allow_self_signed) && ssl[s_MongoDBDriverManager_context_ssl_allow_self_signed].isBoolean()) {
		apply_ssl = true;
		ssl_opt.weak_cert_validation = ssl[s_MongoDBDriverManager_context_ssl_allow_self_signed].toBoolean();
	} else if (ssl.exists(s_MongoDBDriverManager_context_ssl_weak_cert_validation) && ssl[s_MongoDBDriverManager_context_ssl_weak_cert_validation].isBoolean()) {
		apply_ssl = true;
		ssl_opt.weak_cert_validation = ssl[s_MongoDBDriverManager_context_ssl_weak_cert_validation].toBoolean();
	}

	if (ssl.exists(s_MongoDBDriverManager_context_ssl_pem_file) && ssl[s_MongoDBDriverManager_context_ssl_pem_file].isString()) {
		apply_ssl = true;
		ssl_opt.pem_file = ssl[s_MongoDBDriverManager_context_ssl_pem_file].toString().c_str();
	} else if (ssl.exists(s_MongoDBDriverManager_context_ssl_local_cert) && ssl[s_MongoDBDriverManager_context_ssl_local_cert].isString()) {
		apply_ssl = true;
		ssl_opt.pem_file = ssl[s_MongoDBDriverManager_context_ssl_local_cert].toString().c_str();
	}

	if (ssl.exists(s_MongoDBDriverManager_context_ssl_pem_pwd) && ssl[s_MongoDBDriverManager_context_ssl_pem_pwd].isString()) {
		apply_ssl = true;
		ssl_opt.pem_pwd = ssl[s_MongoDBDriverManager_context_ssl_pem_pwd].toString().c_str();
	}  else if (ssl.exists(s_MongoDBDriverManager_context_ssl_passphrase) && ssl[s_MongoDBDriverManager_context_ssl_passphrase].isString()) {
		apply_ssl = true;
		ssl_opt.pem_pwd = ssl[s_MongoDBDriverManager_context_ssl_passphrase].toString().c_str();
	}

	if (ssl.exists(s_MongoDBDriverManager_context_ssl_ca_file) && ssl[s_MongoDBDriverManager_context_ssl_ca_file].isString()) {
		apply_ssl = true;
		ssl_opt.ca_file = ssl[s_MongoDBDriverManager_context_ssl_ca_file].toString().c_str();
	} else if (ssl.exists(s_MongoDBDriverManager_context_ssl_cafile) && ssl[s_MongoDBDriverManager_context_ssl_cafile].isString()) {
		apply_ssl = true;
		ssl_opt.ca_file = ssl[s_MongoDBDriverManager_context_ssl_cafile].toString().c_str();
	}

	if (ssl.exists(s_MongoDBDriverManager_context_ssl_ca_dir) && ssl[s_MongoDBDriverManager_context_ssl_ca_dir].isString()) {
		apply_ssl = true;
		ssl_opt.ca_dir = ssl[s_MongoDBDriverManager_context_ssl_ca_dir].toString().c_str();
	} else if (ssl.exists(s_MongoDBDriverManager_context_ssl_capath) && ssl[s_MongoDBDriverManager_context_ssl_capath].isString()) {
		apply_ssl = true;
		ssl_opt.ca_dir = ssl[s_MongoDBDriverManager_context_ssl_capath].toString().c_str();
	}

	if (ssl.exists(s_MongoDBDriverManager_context_ssl_crl_file) && ssl[s_MongoDBDriverManager_context_ssl_crl_file].isString()) {
		apply_ssl = true;
		ssl_opt.crl_file = ssl[s_MongoDBDriverManager_context_ssl_crl_file].toString().c_str();
	}

	if (apply_ssl) {
		mongoc_client_set_ssl_opts(client, &ssl_opt);
	}
	return 1;
}

#if APM_0
void command_started(const mongoc_apm_command_started_t *event)
{
	ObjectData *obj_context = (ObjectData*) mongoc_apm_command_started_get_context(event);
	Class *cls = obj_context->getVMClass();
	static Class *c_event;
	Func  *m   = cls->lookupMethod(s_MongoDBDriverManager_dispatchCommandStarted.get());
	MongoDBDriverManagerData* data = Native::data<MongoDBDriverManagerData>(obj_context);

	Array subscribers = obj_context->o_get(s_MongoDBDriverManager_subscribers, false, s_MongoDriverManager_className).toArray();

	if (subscribers.size() < 1) {
		return;
	}

	c_event = Unit::lookupClass(s_MongoDriverMonitoringCommandStartedEvent_className.get());
	Object o_event = Object{c_event};
	o_event->o_set(s_MongoDriverMonitoringCommandEvent_commandName, Variant(mongoc_apm_command_started_get_command_name(event)), s_MongoDriverMonitoringCommandEvent_className);
	o_event->o_set(s_MongoDriverMonitoringCommandEvent_server, Variant(hippo_mongo_driver_server_create_from_id(data->m_client, mongoc_apm_command_started_get_server_id(event))), s_MongoDriverMonitoringCommandEvent_className);
	o_event->o_set(s_MongoDriverMonitoringCommandEvent_operationId, Variant(mongoc_apm_command_started_get_operation_id(event)), s_MongoDriverMonitoringCommandEvent_className);
	o_event->o_set(s_MongoDriverMonitoringCommandEvent_requestId, Variant(mongoc_apm_command_started_get_request_id(event)), s_MongoDriverMonitoringCommandEvent_className);
	o_event->o_set(s_MongoDriverMonitoringCommandStartedEvent_databaseName, Variant(mongoc_apm_command_started_get_database_name(event)), s_MongoDriverMonitoringCommandStartedEvent_className);

	/* Add the command */
	Variant v_command;
	const bson_t *b_command = mongoc_apm_command_started_get_command(event);
	hippo_bson_conversion_options_t options = HIPPO_TYPEMAP_INITIALIZER;
	BsonToVariantConverter convertor(bson_get_data(b_command), b_command->len, options);
	convertor.convert(&v_command);
	o_event->o_set(s_MongoDriverMonitoringCommandStartedEvent_command, v_command, s_MongoDriverMonitoringCommandStartedEvent_className);


	TypedValue args[1] = { *(Variant(o_event)).asCell() };

#if HIPPO_HHVM_VERSION >= 31700
	g_context->invokeFuncFew(
		m, obj_context,
		nullptr, 1, args
	);
#else
	Variant result;
	g_context->invokeFuncFew(
		result.asTypedValue(),
		m, obj_context,
		nullptr, 1, args
	);
#endif
}


static void command_succeeded(const mongoc_apm_command_succeeded_t *event)
{
	ObjectData *obj_context = (ObjectData*) mongoc_apm_command_succeeded_get_context(event);
	Class *cls = obj_context->getVMClass();
	static Class *c_event;
	Func  *m   = cls->lookupMethod(s_MongoDBDriverManager_dispatchCommandSucceeded.get());
	MongoDBDriverManagerData* data = Native::data<MongoDBDriverManagerData>(obj_context);

	Array subscribers = obj_context->o_get(s_MongoDBDriverManager_subscribers, false, s_MongoDriverManager_className).toArray();

	if (subscribers.size() < 1) {
		return;
	}

	c_event = Unit::lookupClass(s_MongoDriverMonitoringCommandSucceededEvent_className.get());
	Object o_event = Object{c_event};
	o_event->o_set(s_MongoDriverMonitoringCommandEvent_commandName, Variant(mongoc_apm_command_succeeded_get_command_name(event)), s_MongoDriverMonitoringCommandEvent_className);
	o_event->o_set(s_MongoDriverMonitoringCommandEvent_server, Variant(hippo_mongo_driver_server_create_from_id(data->m_client, mongoc_apm_command_succeeded_get_server_id(event))), s_MongoDriverMonitoringCommandEvent_className);
	o_event->o_set(s_MongoDriverMonitoringCommandEvent_operationId, Variant(mongoc_apm_command_succeeded_get_operation_id(event)), s_MongoDriverMonitoringCommandEvent_className);
	o_event->o_set(s_MongoDriverMonitoringCommandEvent_requestId, Variant(mongoc_apm_command_succeeded_get_request_id(event)), s_MongoDriverMonitoringCommandEvent_className);
	o_event->o_set(s_MongoDriverMonitoringCommandResultEvent_durationMicros, Variant(mongoc_apm_command_succeeded_get_duration(event)), s_MongoDriverMonitoringCommandResultEvent_className);

	/* Add the reply */
	Variant v_reply;
	const bson_t *b_reply = mongoc_apm_command_succeeded_get_reply(event);
	hippo_bson_conversion_options_t options = HIPPO_TYPEMAP_INITIALIZER;
	BsonToVariantConverter convertor(bson_get_data(b_reply), b_reply->len, options);
	convertor.convert(&v_reply);
	o_event->o_set(s_MongoDriverMonitoringCommandSucceededEvent_reply, v_reply, s_MongoDriverMonitoringCommandSucceededEvent_className);

	TypedValue args[1] = { *(Variant(o_event)).asCell() };
	
#if HIPPO_HHVM_VERSION >= 31700
	g_context->invokeFuncFew(
		m, obj_context,
		nullptr, 1, args
	);
#else
	Variant result;
	g_context->invokeFuncFew(
		result.asTypedValue(),
		m, obj_context,
		nullptr, 1, args
	);
#endif
}


static void command_failed(const mongoc_apm_command_failed_t *event)
{
	ObjectData *obj_context = (ObjectData*) mongoc_apm_command_failed_get_context(event);
	Class *cls = obj_context->getVMClass();
	static Class *c_event;
	Func  *m   = cls->lookupMethod(s_MongoDBDriverManager_dispatchCommandFailed.get());
	MongoDBDriverManagerData* data = Native::data<MongoDBDriverManagerData>(obj_context);

	Array subscribers = obj_context->o_get(s_MongoDBDriverManager_subscribers, false, s_MongoDriverManager_className).toArray();

	if (subscribers.size() < 1) {
		return;
	}

	c_event = Unit::lookupClass(s_MongoDriverMonitoringCommandFailedEvent_className.get());
	Object o_event = Object{c_event};
	o_event->o_set(s_MongoDriverMonitoringCommandEvent_commandName, Variant(mongoc_apm_command_failed_get_command_name(event)), s_MongoDriverMonitoringCommandEvent_className);
	o_event->o_set(s_MongoDriverMonitoringCommandEvent_server, Variant(hippo_mongo_driver_server_create_from_id(data->m_client, mongoc_apm_command_failed_get_server_id(event))), s_MongoDriverMonitoringCommandEvent_className);
	o_event->o_set(s_MongoDriverMonitoringCommandEvent_operationId, Variant(mongoc_apm_command_failed_get_operation_id(event)), s_MongoDriverMonitoringCommandEvent_className);
	o_event->o_set(s_MongoDriverMonitoringCommandEvent_requestId, Variant(mongoc_apm_command_failed_get_request_id(event)), s_MongoDriverMonitoringCommandEvent_className);
	o_event->o_set(s_MongoDriverMonitoringCommandResultEvent_durationMicros, Variant(mongoc_apm_command_failed_get_duration(event)), s_MongoDriverMonitoringCommandResultEvent_className);

	/* Add the error / exception */
	bson_error_t b_error;
	mongoc_apm_command_failed_get_error(event, &b_error);
	o_event->o_set(s_MongoDriverMonitoringCommandFailedEvent_error, MongoDriver::Utils::throwExceptionFromBsonError(&b_error), s_MongoDriverMonitoringCommandFailedEvent_className);

	TypedValue args[1] = { *(Variant(o_event)).asCell() };
	
#if HIPPO_HHVM_VERSION >= 31700
	g_context->invokeFuncFew(
		m, obj_context,
		nullptr, 1, args
	);
#else
	Variant result;
	g_context->invokeFuncFew(
		result.asTypedValue(),
		m, obj_context,
		nullptr, 1, args
	);
#endif
}

static void hippo_mongo_driver_manager_set_monitoring_callbacks(mongoc_client_t *client, const ObjectData* this_)
{
	mongoc_apm_callbacks_t *callbacks = mongoc_apm_callbacks_new();

	mongoc_apm_set_command_started_cb(callbacks, command_started);
	mongoc_apm_set_command_succeeded_cb(callbacks, command_succeeded);
	mongoc_apm_set_command_failed_cb(callbacks, command_failed);
	mongoc_client_set_apm_callbacks(client, callbacks, (void *) this_);
	mongoc_apm_callbacks_destroy(callbacks);
}
#endif

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

	if (strcmp(dsn.c_str(), "") == 0) {
		uri = hippo_mongo_driver_manager_make_uri("mongodb://127.0.0.1/", options);
	} else {
		uri = hippo_mongo_driver_manager_make_uri(dsn.c_str(), options);
	}

	data->m_hash = Pool::CreateHash(uri, options, driverOptions);

	hippo_mongo_driver_manager_apply_rc(uri, options);
	hippo_mongo_driver_manager_apply_rp(uri, options);
	hippo_mongo_driver_manager_apply_wc(uri, options);

	if (options.exists(s_MongoDBDriverManager_appname)) {
		if (!mongoc_uri_set_appname(uri, options[s_MongoDBDriverManager_appname].toString().c_str())) {
			mongoc_uri_destroy(uri);
			throw MongoDriver::Utils::throwInvalidArgumentException("Invalid appname value: '" + options[s_MongoDBDriverManager_appname].toString() + "'");
		}
	}

	client = Pool::GetClient(data->m_hash, uri);

	mongoc_uri_destroy(uri);

	if (!client) {
		throw MongoDriver::Utils::throwRunTimeException("Failed to create Manager from URI: '" + dsn + "'");
	}

	data->m_client = client;

	hippo_mongo_driver_manager_apply_ssl_opts(data->m_client, driverOptions);
#ifdef APM_0
	hippo_mongo_driver_manager_set_monitoring_callbacks(data->m_client, this_);
#endif
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
		retval.add(
			(int64_t) i,
			hippo_mongo_driver_server_create_from_id(data->m_client, mongoc_server_description_id(sds[i]))
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
