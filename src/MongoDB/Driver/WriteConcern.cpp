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

#include "../../../libmongoc/src/mongoc/mongoc-write-concern.h"
#define MONGOC_I_AM_A_DRIVER
#include "../../../libmongoc/src/mongoc/mongoc-write-concern-private.h"
#undef MONGOC_I_AM_A_DRIVER

#include "WriteConcern.h"

namespace HPHP {

const StaticString s_MongoDriverWriteConcern_className("MongoDB\\Driver\\WriteConcern");
const StaticString s_MongoDriverWriteConcern_majority("majority");
const StaticString s_MongoDriverWriteConcern_w("w");
const StaticString s_MongoDriverWriteConcern_wmajority("wmajority");
const StaticString s_MongoDriverWriteConcern_wtimeout("wtimeout");
const StaticString s_MongoDriverWriteConcern_fsync("fsync");
const StaticString s_MongoDriverWriteConcern_journal("journal");
Class* MongoDBDriverWriteConcernData::s_class = nullptr;
const StaticString MongoDBDriverWriteConcernData::s_className("MongoDBDriverWriteConcern");
IMPLEMENT_GET_CLASS(MongoDBDriverWriteConcernData);

void HHVM_METHOD(MongoDBDriverWriteConcern, __construct, const Variant &w, const Variant &w_timeout, const Variant &journal, const Variant &fsync)
{
	MongoDBDriverWriteConcernData* data = Native::data<MongoDBDriverWriteConcernData>(this_);
	data->m_write_concern = mongoc_write_concern_new();

	if (w.isInteger()) {
		mongoc_write_concern_set_w(data->m_write_concern, w.toInt64());
	} else if (w.isString()) {
		String w_str = w.toString();

		if (w_str.compare(s_MongoDriverWriteConcern_majority) == 0) {
			mongoc_write_concern_set_w(data->m_write_concern, MONGOC_WRITE_CONCERN_W_MAJORITY);
		} else {
			mongoc_write_concern_set_wtag(data->m_write_concern, w_str.c_str());
		}
	}

	if (!w_timeout.isNull()) {
		int64_t w_int = w_timeout.toInt64();

		mongoc_write_concern_set_wtimeout(data->m_write_concern, w_int);
	}

	if (!journal.isNull()) {
		mongoc_write_concern_set_journal(data->m_write_concern, journal.toBoolean());
	}

	if (!fsync.isNull()) {
		mongoc_write_concern_set_fsync(data->m_write_concern, fsync.toBoolean());
	}
}

Array HHVM_METHOD(MongoDBDriverWriteConcern, __debugInfo)
{
	MongoDBDriverWriteConcernData* data = Native::data<MongoDBDriverWriteConcernData>(this_);
	const char *wtag = mongoc_write_concern_get_wtag(data->m_write_concern);
	const int32_t w = mongoc_write_concern_get_w(data->m_write_concern);

	Array retval = Array::Create();

	if (wtag) {
		retval.set(s_MongoDriverWriteConcern_w, (char*)wtag);
	} else if (mongoc_write_concern_get_wmajority(data->m_write_concern)) {
		retval.set(s_MongoDriverWriteConcern_w, "majority");
	} else if (w != MONGOC_WRITE_CONCERN_W_DEFAULT) {
		retval.set(s_MongoDriverWriteConcern_w, w);
	}

	retval.set(s_MongoDriverWriteConcern_wmajority, mongoc_write_concern_get_wmajority(data->m_write_concern));
	retval.set(s_MongoDriverWriteConcern_wtimeout, mongoc_write_concern_get_wtimeout(data->m_write_concern));

	if (data->m_write_concern->fsync_ != MONGOC_WRITE_CONCERN_FSYNC_DEFAULT) {
		retval.set(s_MongoDriverWriteConcern_fsync, mongoc_write_concern_get_fsync(data->m_write_concern));
	} else {
		retval.set(s_MongoDriverWriteConcern_fsync, Variant());
	}

	if (data->m_write_concern->journal != MONGOC_WRITE_CONCERN_JOURNAL_DEFAULT) {
		retval.set(s_MongoDriverWriteConcern_journal, mongoc_write_concern_get_journal(data->m_write_concern));
	} else {
		retval.set(s_MongoDriverWriteConcern_journal, Variant());
	}

	return retval;
}

}
