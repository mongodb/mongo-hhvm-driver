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

#define MONGOC_I_AM_A_DRIVER
#define delete not_delete
#include "../../../libmongoc/src/mongoc/mongoc-cursor-private.h"
#undef delete
#undef MONGOC_I_AM_A_DRIVER

#include "../../../bson.h"
#include "../../../mongodb.h"

#include "CursorId.h"
#include "Cursor.h"
#include "Server.h"

namespace HPHP {

const StaticString s_MongoDriverCursor_className("MongoDB\\Driver\\Cursor");
Class* MongoDBDriverCursorData::s_class = nullptr;
const StaticString MongoDBDriverCursorData::s_className("MongoDBDriverCursor");
IMPLEMENT_GET_CLASS(MongoDBDriverCursorData);


static void invalidate_current(MongoDBDriverCursorData *data)
{
	if (data->zchild_active) {
		data->zchild_active = false;
	}
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
	ObjectData* obj = ObjectData::newInstance(c_cursor);

	MongoDBDriverCursorIdData* cursorid_data = Native::data<MongoDBDriverCursorIdData>(obj);

	cursorid_data->id = cursorid;

	return Object(obj);
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

static bool hippo_cursor_load_next(MongoDBDriverCursorData* data)
{
	const bson_t *doc;

	if (mongoc_cursor_next(data->cursor, &doc)) {
		Variant v;

		BsonToVariantConverter convertor(bson_get_data(doc), doc->len, true);
		convertor.convert(&v);
		data->zchild_active = true;
		data->zchild = v;

		return true;
	}
	return false;
}

static bool hippo_cursor_next(MongoDBDriverCursorData* data)
{
	invalidate_current(data);

	data->current++;
	if (bson_iter_next(&data->first_batch_iter)) {
		if (BSON_ITER_HOLDS_DOCUMENT(&data->first_batch_iter)) {
			const uint8_t *document = NULL;
			uint32_t document_len = 0;

			bson_iter_document(&data->first_batch_iter, &document_len, &document);

			data->zchild_active = true;
			return true;
		}
	}
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
	invalidate_current(data);
	data->current = 0;
	data->zchild_active = false;

	if (data->first_batch) {
		if (data->is_command_cursor) {
			if (!bson_iter_init(&data->first_batch_iter, data->first_batch)) {
				return;
			}
			if (bson_iter_next(&data->first_batch_iter)) {
				if (BSON_ITER_HOLDS_DOCUMENT(&data->first_batch_iter)) {
					const uint8_t *document = NULL;
					uint32_t document_len = 0;
					Variant v;

					bson_iter_document(&data->first_batch_iter, &document_len, &document);

					BsonToVariantConverter convertor(document, document_len, true);
					convertor.convert(&v);
					data->zchild_active = true;
					data->zchild = v;
				}
			}
		} else {
			Variant v;

			BsonToVariantConverter convertor(bson_get_data(data->first_batch), data->first_batch->len, true);
			convertor.convert(&v);
			data->zchild_active = true;
			data->zchild = v;
		}
	}
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
	static Class* c_server;
	mongoc_host_list_t host;
	MongoDBDriverCursorData* data = Native::data<MongoDBDriverCursorData>(this_);

	mongoc_cursor_get_host(data->cursor, &host);

	/* Prepare result */
	c_server = Unit::lookupClass(s_MongoDriverServer_className.get());
	assert(c_server);
	ObjectData* obj = ObjectData::newInstance(c_server);

	MongoDBDriverServerData* result_data = Native::data<MongoDBDriverServerData>(obj);

	result_data->m_client = data->cursor->client;
	result_data->m_server_id = data->m_server_id;

	return Object(obj);
}


}
