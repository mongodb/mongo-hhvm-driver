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

namespace HPHP {

extern const StaticString s_MongoDriverCursor_className;

class MongoDBDriverCursorData
{
	public:
		static Class* s_class;
		static const StaticString s_className;

		static Class* getClass();

		mongoc_cursor_t *cursor;
		int              hint;
		bool             is_command_cursor;
		bson_t          *first_batch;
		int64_t          current;

		/* Iterators */
		bson_iter_t      first_batch_iter;

		struct visitor_data {
			bool zchild_active;
			Variant zchild;
		} visitor_data;

		void sweep() {
		}

		~MongoDBDriverCursorData() {
			sweep();
		};
};

Object HHVM_METHOD(MongoDBDriverCursor, getId);

Variant HHVM_METHOD(MongoDBDriverCursor, current);
int64_t HHVM_METHOD(MongoDBDriverCursor, key);
Variant HHVM_METHOD(MongoDBDriverCursor, next);
void HHVM_METHOD(MongoDBDriverCursor, rewind);
bool HHVM_METHOD(MongoDBDriverCursor, valid);

}
#endif
