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
std::cout << "start invalidate\n";
std::cout << "C: zchild_active: " << data->zchild_active << "\n";
	if (data->zchild_active) {
//		data->zchild = NULL;
		data->zchild_active = false;
	}
std::cout << "E: zchild_active: " << data->zchild_active << "\n";
std::cout << "end invalidate\n";
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
	std::cout << "current\n";
	MongoDBDriverCursorData* data = Native::data<MongoDBDriverCursorData>(this_);

	return data->zchild;
}

int64_t HHVM_METHOD(MongoDBDriverCursor, key)
{
	std::cout << "key\n";
	MongoDBDriverCursorData* data = Native::data<MongoDBDriverCursorData>(this_);

	return data->current;
}

Variant HHVM_METHOD(MongoDBDriverCursor, next)
{
	std::cout << "next: " << this_ << "\n";
	const bson_t *doc;
	MongoDBDriverCursorData* data = Native::data<MongoDBDriverCursorData>(this_);

	invalidate_current(data);

	data->current++;
	if (bson_iter_next(&data->first_batch_iter)) {
		if (BSON_ITER_HOLDS_DOCUMENT(&data->first_batch_iter)) {
			const uint8_t *document = NULL;
			uint32_t document_len = 0;

			bson_iter_document(&data->first_batch_iter, &document_len, &document);
/*
			MAKE_STD_ZVAL(result->zchild);
			bson_to_zval(document, document_len, &data->;
*/
std::cout << "S: zchild_active: " << data->zchild_active << "\n";
			data->zchild_active = true;
std::cout << "E: zchild_active: " << data->zchild_active << "\n";
			return String("next");
		}
	}
	if (mongoc_cursor_next(data->cursor, &doc)) {
		Variant v;
/*
		MAKE_STD_ZVAL(result->zchild);
		bson_to_zval(bson_get_data(doc), doc->len, &data->;
*/
		BsonToVariantConverter convertor(bson_get_data(doc), doc->len);
		convertor.convert(&v);

std::cout << "S: zchild_active: " << data->zchild_active << "\n";
		data->zchild_active = true;
		data->zchild = v;
std::cout << "E: zchild_active: " << data->zchild_active << "\n";
		return String("next");
	} else {
		invalidate_current(data);
	}

	return String("fail");
}

void HHVM_METHOD(MongoDBDriverCursor, rewind)
{
	std::cout << "rewinding: " << this_ << "\n";
	MongoDBDriverCursorData* data = Native::data<MongoDBDriverCursorData>(this_);

	invalidate_current(data);

	if (data->first_batch) {
		if (data->is_command_cursor) {
			if (!bson_iter_init(&data->first_batch_iter, data->first_batch)) {
				return;
			}
			if (bson_iter_next(&data->first_batch_iter)) {
				if (BSON_ITER_HOLDS_DOCUMENT(&data->first_batch_iter)) {
					const uint8_t *document = NULL;
					uint32_t document_len = 0;

					bson_iter_document(&data->first_batch_iter, &document_len, &document);
/*
					MAKE_STD_ZVAL(result->zchild);
					bson_to_zval(document, document_len, &data->;
*/
std::cout << "S: zchild_active: " << data->zchild_active << "\n";
					data->zchild_active = true;
std::cout << "E: zchild_active: " << data->zchild_active << "\n";
				}
			}
		} else {
/*
			MAKE_STD_ZVAL(result->zchild);
			bson_to_zval(bson_get_data(result->firstBatch), result->firstBatch->len, &result->;
*/
std::cout << "S: zchild_active: " << data->zchild_active << "\n";
			data->zchild_active = true;
std::cout << "E: zchild_active: " << data->zchild_active << "\n";
		}
	}
}

bool HHVM_METHOD(MongoDBDriverCursor, valid)
{
	std::cout << "valid: " << this_ << "\n";
	MongoDBDriverCursorData* data = Native::data<MongoDBDriverCursorData>(this_);

std::cout << "C: zchild_active: " << data->zchild_active << "\n";
	if (data->zchild_active) {
		return true;
	}

	return false;
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

	result_data->hint = data->hint;
	result_data->host = &host;

	return Object(obj);
}


}
