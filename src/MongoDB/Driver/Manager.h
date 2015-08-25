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

#include "../../../utils.h"

namespace HPHP {

class MongoDBDriverManagerData
{
	public:
		static Class* s_class;
		static const StaticString s_className;

		mongoc_client_t *m_client;

		static Class* getClass();

		MongoDBDriverManagerData() {
			m_client = NULL;
		}

		void wakeup (const Variant& context, ObjectData* obj) {
			throw MongoDriver::Utils::throwRunTimeException("Unserialization of MongoDB\\Driver\\Manager is not allowed");
		}

		Variant sleep() const {
			throw MongoDriver::Utils::throwRunTimeException("Serialization of MongoDB\\Driver\\Manager is not allowed");
		}

		void sweep() {
			mongoc_client_destroy(m_client);
		}

		~MongoDBDriverManagerData() {
			sweep();
		};
};

void HHVM_METHOD(MongoDBDriverManager, __construct, const String &dsn, const Array &options, const Array &driverOptions);
Array HHVM_METHOD(MongoDBDriverManager, __debugInfo);
void HHVM_METHOD(MongoDBDriverManager, __wakeup);
Object HHVM_METHOD(MongoDBDriverManager, executeBulkWrite, const String &ns, const Object &bulk, const Variant &writeConcern);
Object HHVM_METHOD(MongoDBDriverManager, executeCommand, const String &db, const Object &command, const Variant &readPreference);
Object HHVM_METHOD(MongoDBDriverManager, executeDelete, const String &ns, const Variant &query, const Variant &deleteOptions, const Variant &writeConcern);
Object HHVM_METHOD(MongoDBDriverManager, executeInsert, const String &ns, const Variant &document, const Variant &writeConcern);
Object HHVM_METHOD(MongoDBDriverManager, executeQuery, const String &ns, const Object &query, const Variant &readPreference);
Object HHVM_METHOD(MongoDBDriverManager, executeUpdate, const String &ns, const Variant &query, const Variant &newObj, const Variant &updateOptions, const Variant &writeConcern);
Array HHVM_METHOD(MongoDBDriverManager, getReadPreference);
Array HHVM_METHOD(MongoDBDriverManager, getWriteConcern);
Object HHVM_METHOD(MongoDBDriverManager, selectServer, const Object &readPreference);

}
#endif
