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
#ifndef __MONGODB_DRIVER_CURSORID_H__
#define __MONGODB_DRIVER_CURSORID_H__
namespace HPHP {

extern const StaticString s_MongoDriverCursorId_className;

class MongoDBDriverCursorIdData
{
	public:
		static Class* s_class;
		static const StaticString s_className;

		static Class* getClass();

		int64_t id;

		void sweep() {
		}

		~MongoDBDriverCursorIdData() {
			sweep();
		};
};

void HHVM_METHOD(MongoDBDriverCursorId, __construct, const String &id);
String HHVM_METHOD(MongoDBDriverCursorId, __toString);
Array HHVM_METHOD(MongoDBDriverCursorId, __debugInfo);

}
#endif
