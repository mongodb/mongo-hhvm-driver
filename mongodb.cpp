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

extern "C" {
#include "libbson/src/bson/bson.h"
#include "libmongoc/src/mongoc/mongoc.h"
#include <stdio.h>
}

#define IMPLEMENT_GET_CLASS(cls) \
	Class* cls::getClass() { \
		if (s_class == nullptr) { \
			s_class = Unit::lookupClass(s_className.get()); \
			assert(s_class); \
		} \
		return s_class; \
	}

namespace HPHP {

class MongoDBManagerData
{
	public:
		static Class* s_class;
		static const StaticString s_className;

		mongoc_client_t *m_client;

		static Class* getClass();

		void sweep() {
			mongoc_client_destroy(m_client);
		}

		~MongoDBManagerData() {
			sweep();
		};
};

Class* MongoDBManagerData::s_class = nullptr;
const StaticString MongoDBManagerData::s_className("MongoDBManager");
IMPLEMENT_GET_CLASS(MongoDBManagerData);

static void HHVM_METHOD(MongoDBManager, __construct, const String &dsn, const Array &options, const Array &driverOptions)
{
	MongoDBManagerData* data = Native::data<MongoDBManagerData>(this_);

	mongoc_client_t *client;
/*
    mongoc_collection_t *collection;
    mongoc_cursor_t *cursor;
    bson_error_t error;
    bson_oid_t oid;
    bson_t *doc;
*/
	client = mongoc_client_new(dsn.c_str());

	if (!client) {
		throw Object(SystemLib::AllocExceptionObject("Can't connect"));
	}

	data->m_client = client;
/*
    doc = bson_new ();
    bson_oid_init (&oid, NULL);
    BSON_APPEND_OID (doc, "_id", &oid);
    BSON_APPEND_UTF8 (doc, "hello", "world");

    if (!mongoc_collection_insert (collection, MONGOC_INSERT_NONE, doc, NULL, &error)) {
        printf ("%s\n", error.message);
    }

    bson_destroy (doc);
    mongoc_collection_destroy (collection);
    mongoc_client_destroy (client);
*/
}

const StaticString s_MongoDriverWriteResult_className("MongoDB\\Driver\\WriteResult");

static Object HHVM_METHOD(MongoDBManager, executeInsert, const String &ns, const Variant &document, const Object &writeConcern)
{
	static Class* c_foobar;

	c_foobar = Unit::lookupClass(s_MongoDriverWriteResult_className.get());
	assert(c_foobar);
	ObjectData* obj = ObjectData::newInstance(c_foobar);

	return Object(obj);
}

static class MongoDBExtension : public Extension {
	public:
		MongoDBExtension() : Extension("mongodb") {}

		virtual void moduleInit() {
			HHVM_MALIAS(MongoDB\\Manager, __construct, MongoDBManager, __construct);
			HHVM_MALIAS(MongoDB\\Manager, executeInsert, MongoDBManager, executeInsert);

			Native::registerNativeDataInfo<MongoDBManagerData>(MongoDBManagerData::s_className.get());

			loadSystemlib("mongodb");
			mongoc_init();
		}
} s_mongodb_extension;

HHVM_GET_MODULE(mongodb)

} // namespace HPHP
