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
#ifndef __MONGODB_DRIVER_READPREFERENCE_H__
#define __MONGODB_DRIVER_READPREFERENCE_H__

#include "../../../mongodb.h"
#include "../../../bson.h"

extern "C" {
#include "../../../libbson/src/bson/bson.h"
#include "../../../libmongoc/src/mongoc/mongoc.h"
}

namespace HPHP {

extern const StaticString s_MongoDriverReadPreference_className;

class MongoDBDriverReadPreferenceData
{
	public:
		static Class* s_class;
		static const StaticString s_className;

		mongoc_read_prefs_t *m_read_preference = NULL;

		static Class* getClass();

		void sweep() {
			mongoc_read_prefs_destroy(m_read_preference);
		}

		~MongoDBDriverReadPreferenceData() {
			sweep();
		};
};

void HHVM_METHOD(MongoDBDriverReadPreference, _setReadPreference, int readPreference);
void HHVM_METHOD(MongoDBDriverReadPreference, _setReadPreferenceTags, const Array &tagSets);
Array HHVM_METHOD(MongoDBDriverReadPreference, __debugInfo);
Array HHVM_METHOD(MongoDBDriverReadPreference, bsonSerialize);
int64_t HHVM_METHOD(MongoDBDriverReadPreference, getMode);
Array HHVM_METHOD(MongoDBDriverReadPreference, getTagSets);

}
#endif
