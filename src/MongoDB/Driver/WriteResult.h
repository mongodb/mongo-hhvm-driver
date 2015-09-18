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
#ifndef __MONGODB_DRIVER_WRITERESULT_H__
#define __MONGODB_DRIVER_WRITERESULT_H__

#define MONGOC_I_AM_A_DRIVER
#include "../../../libmongoc/src/mongoc/mongoc-bulk-operation-private.h"
#undef MONGOC_I_AM_A_DRIVER

namespace HPHP {

extern const StaticString s_MongoDriverWriteResult_className;

class MongoDBDriverWriteResultData
{
	public:
		static Class* s_class;
		static const StaticString s_className;

		/* properties go here */
		mongoc_client_t    *m_client;
		uint32_t            m_server_id;
		mongoc_write_concern_t *m_write_concern;

		static Class* getClass();

		void sweep() {
			/* do nothing */
		}

		~MongoDBDriverWriteResultData() {
			sweep();
		};
};

Object HHVM_METHOD(MongoDBDriverWriteResult, getServer);
bool HHVM_METHOD(MongoDBDriverWriteResult, isAcknowledged);

Object hippo_write_result_init(mongoc_write_result_t *write_result, mongoc_client_t *client, int server_id, int success, const mongoc_write_concern_t *write_concern, bool unwrap_bw_exception);

}
#endif
