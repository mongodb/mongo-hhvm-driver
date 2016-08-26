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

#undef TRACE

#include "../../../bson.h"
#include "../../../utils.h"
#include "../../../mongodb.h"

#include "CursorId.h"
#include "Cursor.h"
#include "Server.h"

namespace HPHP {

const StaticString s_MongoDriverCursor_className("MongoDB\\Driver\\Cursor");
Class* MongoDBDriverCursorData::s_class = nullptr;
const StaticString MongoDBDriverCursorData::s_className("MongoDBDriverCursor");
const StaticString s_MongoDBDriverCursor_database("database");
const StaticString s_MongoDBDriverCursor_collection("collection");
const StaticString s_MongoDBDriverCursor_query("query");
const StaticString s_MongoDBDriverCursor_command("command");
const StaticString s_MongoDBDriverCursor_readPreference("readPreference");
const StaticString s_MongoDBDriverCursor_isDead("isDead");
const StaticString s_MongoDBDriverCursor_currentIndex("currentIndex");
const StaticString s_MongoDBDriverCursor_currentDocument("currentDocument");
const StaticString s_MongoDBDriverCursor_server("server");
IMPLEMENT_GET_CLASS(MongoDBDriverCursorData);

static bool hippo_cursor_load_current(MongoDBDriverCursorData* data);

Object hippo_cursor_init(mongoc_cursor_t *cursor, mongoc_client_t *client, const Variant &readPreference)
{
	static HPHP::Class* c_result;

	c_result = HPHP::Unit::lookupClass(HPHP::s_MongoDriverCursor_className.get());
	assert(c_result);
	HPHP::Object obj = HPHP::Object{c_result};

	HPHP::MongoDBDriverCursorData* cursor_data = HPHP::Native::data<HPHP::MongoDBDriverCursorData>(obj.get());

	cursor_data->cursor = cursor;
	cursor_data->client = client;
	cursor_data->m_server_id = mongoc_cursor_get_hint(cursor);
	cursor_data->next_after_rewind = 0;
	cursor_data->m_read_preference = readPreference;
	cursor_data->m_db = NULL;
	cursor_data->m_collection = NULL;
	cursor_data->current = -1;

	hippo_cursor_load_current(cursor_data);

	return obj;
}

Object hippo_cursor_init_for_command(mongoc_cursor_t *cursor, mongoc_client_t *client, const char *db, const Variant &command, const Variant &readPreference)
{
	auto tmp = hippo_cursor_init(cursor, client, readPreference);
	HPHP::MongoDBDriverCursorData* cursor_data = HPHP::Native::data<HPHP::MongoDBDriverCursorData>(tmp.get());

	cursor_data->m_db = strdup(db);
	cursor_data->m_command = command;

	return tmp;
}

Object hippo_cursor_init_for_query(mongoc_cursor_t *cursor, mongoc_client_t *client, const String &ns, const Object &query, const Variant &readPreference)
{
	auto tmp = hippo_cursor_init(cursor, client, readPreference);
	HPHP::MongoDBDriverCursorData* cursor_data = HPHP::Native::data<HPHP::MongoDBDriverCursorData>(tmp.get());

	MongoDriver::Utils::splitNamespace(ns, &cursor_data->m_db, &cursor_data->m_collection);

	cursor_data->m_query = query;

	return tmp;
}

static void invalidate_current(MongoDBDriverCursorData *data)
{
	if (data->zchild_active) {
		data->zchild_active = false;
	}
}

Array HHVM_METHOD(MongoDBDriverCursor, __debugInfo)
{
	MongoDBDriverCursorData* data = Native::data<MongoDBDriverCursorData>(this_);
	Array retval = Array::Create();

	if (data->m_db) {
		retval.add(s_MongoDBDriverCursor_database, data->m_db);
	} else {
		retval.add(s_MongoDBDriverCursor_database, Variant());
	}
	if (data->m_collection) {
		retval.add(s_MongoDBDriverCursor_collection, data->m_collection);
	} else {
		retval.add(s_MongoDBDriverCursor_collection, Variant());
	}

	if (!data->m_query.isNull()) {
		retval.add(s_MongoDBDriverCursor_query, data->m_query);
	} else {
		retval.add(s_MongoDBDriverCursor_query, Variant());
	}

	if (!data->m_command.isNull()) {
		retval.add(s_MongoDBDriverCursor_command, data->m_command);
	} else {
		retval.add(s_MongoDBDriverCursor_command, Variant());
	}

	if (!data->m_read_preference.isNull()) {
		retval.add(s_MongoDBDriverCursor_readPreference, data->m_read_preference);
	} else {
		retval.add(s_MongoDBDriverCursor_readPreference, Variant());
	}

	retval.add(s_MongoDBDriverCursor_isDead, !mongoc_cursor_is_alive(data->cursor));

	if (data->zchild_active && data->current != -1) {
		retval.add(s_MongoDBDriverCursor_currentIndex, data->current);
		retval.add(s_MongoDBDriverCursor_currentDocument, data->zchild);
	} else {
		retval.add(s_MongoDBDriverCursor_currentIndex, 0);
		retval.add(s_MongoDBDriverCursor_currentDocument, Variant());
	}

	retval.add(s_MongoDBDriverCursor_server, hippo_mongo_driver_server_create_from_id(data->client, data->m_server_id));

	return retval;
}


Object HHVM_METHOD(MongoDBDriverCursor, getId)
{
	static Class* c_cursor;
	int64_t cursorid;
	MongoDBDriverCursorData* data = Native::data<MongoDBDriverCursorData>(this_);

	cursorid = mongoc_cursor_get_id(data->cursor);

	/* Prepare result */
	c_cursor = Unit::lookupClass(s_MongoDriverCursorId_className.get());
	assert(c_cursor);
	Object obj = Object{c_cursor};

	MongoDBDriverCursorIdData* cursorid_data = Native::data<MongoDBDriverCursorIdData>(obj.get());

	cursorid_data->id = cursorid;

	return obj;
}

Variant HHVM_METHOD(MongoDBDriverCursor, current)
{
	MongoDBDriverCursorData* data = Native::data<MongoDBDriverCursorData>(this_);

	return data->zchild;
}

int64_t HHVM_METHOD(MongoDBDriverCursor, key)
{
	MongoDBDriverCursorData* data = Native::data<MongoDBDriverCursorData>(this_);

	return data->current;
}

bool hippo_cursor_load_current(MongoDBDriverCursorData* data)
{
	const bson_t *doc;

	invalidate_current(data);

	doc = mongoc_cursor_current(data->cursor);
	if (doc) {
		Variant v;

		BsonToVariantConverter convertor(bson_get_data(doc), doc->len, data->bson_options);
		convertor.convert(&v);
		data->zchild_active = true;
		data->zchild = v;

		return true;
	}
	return false;
}

static bool hippo_cursor_load_next(MongoDBDriverCursorData* data)
{
	const bson_t *doc;

	if (mongoc_cursor_next(data->cursor, &doc)) {
		return hippo_cursor_load_current(data);
	}
	return false;
}

static bool hippo_cursor_next(MongoDBDriverCursorData* data)
{
	invalidate_current(data);

	data->next_after_rewind++;

	data->current++;
	if (hippo_cursor_load_next(data)) {
		return true;
	} else {
		invalidate_current(data);
	}

	return false;
}

Variant HHVM_METHOD(MongoDBDriverCursor, next)
{
	MongoDBDriverCursorData* data = Native::data<MongoDBDriverCursorData>(this_);

	return hippo_cursor_next(data) ? String("next") : String("fail");
}

static void hippo_cursor_rewind(MongoDBDriverCursorData* data)
{
	if (data->next_after_rewind != 0) {
		if (data->zchild_active) {
			throw MongoDriver::Utils::throwLogicException("Cursors cannot rewind after starting iteration");
		} else { /* If we're not active, image we're now have fully iterated */
			throw MongoDriver::Utils::throwLogicException("Cursors cannot yield multiple iterators");
		}
	}

	data->current = 0;
}

void HHVM_METHOD(MongoDBDriverCursor, rewind)
{
	MongoDBDriverCursorData* data = Native::data<MongoDBDriverCursorData>(this_);

	hippo_cursor_rewind(data);
}

bool HHVM_METHOD(MongoDBDriverCursor, valid)
{
	MongoDBDriverCursorData* data = Native::data<MongoDBDriverCursorData>(this_);

	if (data->zchild_active) {
		return true;
	}

	return false;
}

bool HHVM_METHOD(MongoDBDriverCursor, isDead)
{
	MongoDBDriverCursorData* data = Native::data<MongoDBDriverCursorData>(this_);

	return !(mongoc_cursor_is_alive(data->cursor));
}

Array HHVM_METHOD(MongoDBDriverCursor, toArray)
{
	MongoDBDriverCursorData* data = Native::data<MongoDBDriverCursorData>(this_);
	Array retval = Array::Create();

	hippo_cursor_rewind(data);

	while (data->zchild_active) {
		retval.add(data->current, data->zchild);
		hippo_cursor_next(data);
	}

	return retval;
}

Object HHVM_METHOD(MongoDBDriverCursor, getServer)
{
	MongoDBDriverCursorData* data = Native::data<MongoDBDriverCursorData>(this_);

	return hippo_mongo_driver_server_create_from_id(data->client, data->m_server_id);
}


void HHVM_METHOD(MongoDBDriverCursor, setTypeMap, const Array &typemap)
{
	MongoDBDriverCursorData* data = Native::data<MongoDBDriverCursorData>(this_);

	parseTypeMap(&data->bson_options, typemap);

	/* Reconvert the current document to use the new typemap */
	hippo_cursor_load_current(data);
}

}
