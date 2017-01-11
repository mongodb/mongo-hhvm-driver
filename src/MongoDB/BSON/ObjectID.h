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
#ifndef __MONGODB_BSON_OBJECTID_H__
#define __MONGODB_BSON_OBJECTID_H__
namespace HPHP {

extern "C" {
#include "../../../libbson/src/bson/bson.h"
}

extern const StaticString s_MongoBsonObjectID_className;
extern const StaticString s_MongoBsonObjectID_shortName;

class MongoDBBsonObjectIDData
{
	public:
		static Class* s_class;
		static const StaticString s_className;

		static Class* getClass();

		bson_oid_t m_oid;

		void sweep() {
		}

		~MongoDBBsonObjectIDData() {
			sweep();
		};
};

Object createMongoBsonObjectIDObject(const bson_oid_t *v_oid);

#if HIPPO_HHVM_VERSION >= 31700
void HHVM_METHOD(MongoDBBsonObjectID, __construct, const Variant &objectId = uninit_variant);
#else
void HHVM_METHOD(MongoDBBsonObjectID, __construct, const Variant &objectId = null_variant);
#endif
String HHVM_METHOD(MongoDBBsonObjectID, __toString);
Array HHVM_METHOD(MongoDBBsonObjectID, __debugInfo);

}
#endif

