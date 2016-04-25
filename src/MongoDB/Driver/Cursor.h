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
#ifndef __MONGODB_DRIVER_CURSOR_H__
#define __MONGODB_DRIVER_CURSOR_H__

extern "C" {
#include "../../../libmongoc/src/mongoc/mongoc.h"
}

#include "../../../bson.h"

namespace HPHP {

extern const StaticString s_MongoDriverCursor_className;

class MongoDBDriverCursorData
{
	public:
		static Class* s_class;
		static const StaticString s_className;

		static Class* getClass();

		mongoc_cursor_t *cursor;
		mongoc_client_t *client;
		int              m_server_id;
		int64_t          current;
		int              next_after_rewind = 0;

		/* Conversion & Flags */
		int zchild_active;
		Variant zchild;
		hippo_bson_conversion_options_t bson_options;

		void sweep() {
			mongoc_cursor_destroy(cursor);
		}

		MongoDBDriverCursorData() {
			bson_options = HIPPO_TYPEMAP_INITIALIZER;
		}

		~MongoDBDriverCursorData() {
			sweep();
		};
};

Array HHVM_METHOD(MongoDBDriverCursor, __debugInfo);

Object HHVM_METHOD(MongoDBDriverCursor, getId);
Object HHVM_METHOD(MongoDBDriverCursor, getServer);
void HHVM_METHOD(MongoDBDriverCursor, setTypeMap, const Array &typemap);

Variant HHVM_METHOD(MongoDBDriverCursor, current);
int64_t HHVM_METHOD(MongoDBDriverCursor, key);
Variant HHVM_METHOD(MongoDBDriverCursor, next);
void HHVM_METHOD(MongoDBDriverCursor, rewind);
bool HHVM_METHOD(MongoDBDriverCursor, valid);
bool HHVM_METHOD(MongoDBDriverCursor, isDead);

Array HHVM_METHOD(MongoDBDriverCursor, toArray);

Object hippo_cursor_init(mongoc_cursor_t *cursor, mongoc_client_t *client);

}
#endif
