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
#ifndef __MONGODB_DRIVER_POOL_H__
#define __MONGODB_DRIVER_POOL_H__

extern "C" {
#include "../../../libmongoc/src/mongoc/mongoc.h"
}

namespace HPHP {

class Pool
{
	public:
		mongoc_client_t *m_client;

		Pool(const mongoc_uri_t *uri)
		{
			m_client = mongoc_client_new_from_uri(uri);
		}

		static std::string CreateHash(const mongoc_uri_t *uri, const Array &options, const Array &driverOptions);

		static mongoc_client_t *GetClient(std::string hash, mongoc_uri_t *uri);
		static void ReturnClient(const std::string hash, mongoc_client_t *client);
};

}
#endif
