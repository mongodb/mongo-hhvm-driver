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
#ifndef __MONGODB_DRIVER_MANAGER_H__
#define __MONGODB_DRIVER_MANAGER_H__

extern "C" {
#include "../../../libbson/src/bson/bson.h"
#include "../../../libmongoc/src/mongoc/mongoc.h"
}

namespace HPHP {

class MongoDBDriverManagerData
{
	public:
		static Class* s_class;
		static const StaticString s_className;

		mongoc_client_t *m_client;

		static Class* getClass();

		void sweep() {
			mongoc_client_destroy(m_client);
		}

		~MongoDBDriverManagerData() {
			sweep();
		};
};

void HHVM_METHOD(MongoDBDriverManager, __construct, const String &dsn, const Array &options, const Array &driverOptions);
Object HHVM_METHOD(MongoDBDriverManager, executeInsert, const String &ns, const Variant &document, const Object &writeConcern);
Object HHVM_METHOD(MongoDBDriverManager, executeQuery, const String &ns, Object &query, Object &readPreference);

}
#endif
