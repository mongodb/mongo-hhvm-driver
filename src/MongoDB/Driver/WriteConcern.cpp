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
#include "hphp/runtime/base/type-string.h"
#include "hphp/runtime/base/string-buffer.h"

#include "../../../mongodb.h"
#include "../../../utils.h"

#include "../../../libmongoc/src/mongoc/mongoc-write-concern.h"
#define MONGOC_I_AM_A_DRIVER
#include "../../../libmongoc/src/mongoc/mongoc-write-concern-private.h"
#undef MONGOC_I_AM_A_DRIVER

#include "WriteConcern.h"

namespace HPHP {

const StaticString s_MongoDriverWriteConcern_className("MongoDB\\Driver\\WriteConcern");
const StaticString s_MongoDriverWriteConcern_debugInfo("__debugInfo");
const StaticString s_MongoDriverWriteConcern_majority("majority");
const StaticString s_MongoDriverWriteConcern_w("w");
const StaticString s_MongoDriverWriteConcern_wmajority("wmajority");
const StaticString s_MongoDriverWriteConcern_wtimeout("wtimeout");
const StaticString s_MongoDriverWriteConcern_journal("journal");
Class* MongoDBDriverWriteConcernData::s_class = nullptr;
const StaticString MongoDBDriverWriteConcernData::s_className("MongoDBDriverWriteConcern");
IMPLEMENT_GET_CLASS(MongoDBDriverWriteConcernData);

void HHVM_METHOD(MongoDBDriverWriteConcern, __construct, const Variant &w, const Variant &w_timeout, const Variant &journal)
{
	MongoDBDriverWriteConcernData* data = Native::data<MongoDBDriverWriteConcernData>(this_);
	data->m_write_concern = mongoc_write_concern_new();

	if (w.isInteger()) {
		int64_t w_int = w.toInt64();

		if (w_int < -3) {
			StringBuffer buf;

			buf.printf(
				"Expected w to be >= -3, %ld given",
				w_int
			);
			Variant w_error = buf.detach();
			throw MongoDriver::Utils::throwInvalidArgumentException((char*) w_error.toString().c_str());
		}

		mongoc_write_concern_set_w(data->m_write_concern, w_int);
	} else if (w.isString()) {
		String w_str = w.toString();

		if (w_str.compare(s_MongoDriverWriteConcern_majority) == 0) {
			mongoc_write_concern_set_w(data->m_write_concern, MONGOC_WRITE_CONCERN_W_MAJORITY);
		} else {
			mongoc_write_concern_set_wtag(data->m_write_concern, w_str.c_str());
		}
	} else {
		StringBuffer buf;
		buf.printf(
			"Expected w to be integer or string, %s given",
			getDataTypeString(w.getType()).data()
		);
		Variant type = buf.detach();
		throw MongoDriver::Utils::throwInvalidArgumentException((char*) type.toString().c_str());
	}

	if (!w_timeout.isNull()) {
		int64_t wtimeout_int = w_timeout.toInt64();

		if (wtimeout_int < 0) {
			StringBuffer buf;

			buf.printf(
				"Expected wtimeout to be >= 0, %ld given",
				wtimeout_int
			);
			Variant wtimeout_error = buf.detach();
			throw MongoDriver::Utils::throwInvalidArgumentException((char*) wtimeout_error.toString().c_str());
		}

		mongoc_write_concern_set_wtimeout(data->m_write_concern, wtimeout_int);
	}

	if (!journal.isNull()) {
		mongoc_write_concern_set_journal(data->m_write_concern, journal.toBoolean());
	}
}

bool mongodb_driver_add_write_concern_debug(mongoc_write_concern_t *wc, Array *retval)
{
	const char *wtag = mongoc_write_concern_get_wtag(wc);
	const int32_t w = mongoc_write_concern_get_w(wc);

	if (wtag) {
		retval->set(s_MongoDriverWriteConcern_w, (char*)wtag);
	} else if (mongoc_write_concern_get_wmajority(wc)) {
		retval->set(s_MongoDriverWriteConcern_w, "majority");
	} else if (w != MONGOC_WRITE_CONCERN_W_DEFAULT) {
		retval->set(s_MongoDriverWriteConcern_w, w);
	} else {
		retval->set(s_MongoDriverWriteConcern_w, Variant());
	}

	retval->set(s_MongoDriverWriteConcern_wmajority, mongoc_write_concern_get_wmajority(wc));
	retval->set(s_MongoDriverWriteConcern_wtimeout, mongoc_write_concern_get_wtimeout(wc));

	if (wc->journal != MONGOC_WRITE_CONCERN_JOURNAL_DEFAULT) {
		retval->set(s_MongoDriverWriteConcern_journal, mongoc_write_concern_get_journal(wc));
	} else {
		retval->set(s_MongoDriverWriteConcern_journal, Variant());
	}

	return true;
}

Variant HHVM_METHOD(MongoDBDriverWriteConcern, getJournal)
{
	MongoDBDriverWriteConcernData* data = Native::data<MongoDBDriverWriteConcernData>(this_);
	mongoc_write_concern_t *wc = data->m_write_concern;

	if (wc->journal != MONGOC_WRITE_CONCERN_JOURNAL_DEFAULT) {
		return !!mongoc_write_concern_get_journal(wc);
	} else {
		return Variant();
	}
}

Variant HHVM_METHOD(MongoDBDriverWriteConcern, getW)
{
	MongoDBDriverWriteConcernData* data = Native::data<MongoDBDriverWriteConcernData>(this_);
	mongoc_write_concern_t *wc = data->m_write_concern;
	const char *wtag = mongoc_write_concern_get_wtag(wc);
	const int32_t w = mongoc_write_concern_get_w(wc);

	if (wtag) {
		return Variant(wtag);
	} else if (mongoc_write_concern_get_wmajority(wc)) {
		return Variant("majority");
	} else if (w != MONGOC_WRITE_CONCERN_W_DEFAULT) {
		return Variant((int64_t) w);
	} else {
		return Variant();
	}
}

int64_t HHVM_METHOD(MongoDBDriverWriteConcern, getWtimeout)
{
	MongoDBDriverWriteConcernData* data = Native::data<MongoDBDriverWriteConcernData>(this_);
	mongoc_write_concern_t *wc = data->m_write_concern;

	return (int64_t) mongoc_write_concern_get_wtimeout(wc);
}

Array HHVM_METHOD(MongoDBDriverWriteConcern, __debugInfo)
{
	MongoDBDriverWriteConcernData* data = Native::data<MongoDBDriverWriteConcernData>(this_);

	Array retval = Array::Create();

	mongodb_driver_add_write_concern_debug(data->m_write_concern, &retval);

	return retval;
}

}
