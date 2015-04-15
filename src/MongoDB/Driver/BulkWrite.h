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
#ifndef __MONGODB_DRIVER_BULKWRITE_H__
#define __MONGODB_DRIVER_BULKWRITE_H__

extern "C" {
#include "../../../libbson/src/bson/bson.h"
#include "../../../libmongoc/src/mongoc/mongoc.h"
}

namespace HPHP {

class MongoDBDriverBulkWriteData
{
	public:
		static Class* s_class;
		static const StaticString s_className;

		mongoc_bulk_operation_t *m_bulk;

		static Class* getClass();

		void sweep() {
			mongoc_bulk_operation_destroy(m_bulk);
		}

		~MongoDBDriverBulkWriteData() {
			sweep();
		};
};

void HHVM_METHOD(MongoDBDriverBulkWrite, __construct, const Variant &ordered);
Object HHVM_METHOD(MongoDBDriverBulkWrite, insert, const Variant &document);
void HHVM_METHOD(MongoDBDriverBulkWrite, update, const Variant &query, const Variant &update, const Variant &updateOptions);
void HHVM_METHOD(MongoDBDriverBulkWrite, delete, const Variant &query, const Variant &deleteOptions);
int64_t HHVM_METHOD(MongoDBDriverBulkWrite, count);
Array HHVM_METHOD(MongoDBDriverBulkWrite, __debugInfo);

}
#endif
