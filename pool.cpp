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

#include "hphp/runtime/ext/extension.h"
#include "hphp/runtime/vm/native-data.h"
#include "hphp/runtime/base/array-iterator.h"
#include "hphp/runtime/ext/stream/ext_stream.h"
#include "hphp/runtime/base/variable-serializer.h"

#include "mongodb.h"
#include "pool.h"

extern "C" {
#include "../../../libmongoc/src/mongoc/mongoc-client.h"
}

namespace {
	thread_local std::unordered_map<std::string, std::shared_ptr<HPHP::Pool>> s_connections;
}

namespace HPHP {

std::string Pool::CreateHash(const mongoc_uri_t *uri, const Array &options, const Array &driverOptions)
{
	std::string hash;
	StringBuffer buf;

	VariableSerializer vs(VariableSerializer::Type::Serialize);

	buf.append(mongoc_uri_get_string(uri));

	if (options.size() > 0) {
		buf.append("#");
		buf.append(vs.serialize(Variant(options), true, true));
	}

	if (driverOptions.size() > 0) {
		buf.append("#");
	}
	for (ArrayIter iter(driverOptions); iter; ++iter) {
		Variant key(iter.first());
		String s_key = key.toString();
		Variant value(iter.second());

		if (strcmp(s_key.c_str(), "context") == 0 && value.isResource()) {
			Array context;
			buf.append("context:");
#if HIPPO_HHVM_VERSION >= 30900
			req::ptr<StreamContext> sc;
#else
			SmartPtr<StreamContext> sc;
#endif
			sc = cast<StreamContext>(value);

			if (!sc) {
				continue;
			}

			context = sc->getOptions();
			buf.append(vs.serialize(context, true, true));
		} else {
			buf.append(s_key);
			buf.append(":");

			buf.append(vs.serialize(value, true, true));
		}
	}

	String tmp = buf.detach();

	return tmp.c_str();
}

mongoc_client_t *Pool::GetClient(std::string hash, mongoc_uri_t *uri)
{
	auto key = hash;
	std::shared_ptr<Pool> tmp;

	tmp = s_connections[key];
	if (!tmp) {
		Logger::Verbose("[HIPPO] Client Caching; New Client; Hash: %s", hash.c_str());
		std::shared_ptr<Pool> new_client_ptr(new Pool(uri));
		s_connections[key] = new_client_ptr;
		return new_client_ptr->m_client;
	} else {
		Logger::Verbose("[HIPPO] Client Caching; Existing Client; Hash: %s", hash.c_str());
		return tmp->m_client;
	}
}

void Pool::ReturnClient(const std::string hash, mongoc_client_t *client)
{
	auto key = hash;
	std::shared_ptr<Pool> tmp;

	tmp = s_connections[key];
	if (!tmp) {
		Logger::Warning("[HIPPO] Client Caching; Gone From Map; Hash: %s", hash.c_str());
	} else {
		/* We don't really need to do anything. The anti-case is just to make
		 * sure that things weren't broken. */
	}
}

}
