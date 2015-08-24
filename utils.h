/**
 *  Copyright 2015 MongoDB, Inc.
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
#ifndef __MONGODB_UTILS_H__
#define __MONGODB_UTILS_H__

#include "hphp/runtime/ext/extension.h"

extern "C" {
#include "libbson/src/bson/bson.h"
#include "libmongoc/src/mongoc/mongoc.h"

#define MONGOC_I_AM_A_DRIVER
#include "libmongoc/src/mongoc/mongoc-cursor-cursorid-private.h"
#undef MONGOC_I_AM_A_DRIVER
}

namespace MongoDriver {

extern const HPHP::StaticString s_MongoDriverExceptionAuthenticationException_className;
extern const HPHP::StaticString s_MongoDriverExceptionConnectionException_className;
extern const HPHP::StaticString s_MongoDriverExceptionConnectionTimeoutException_className;
extern const HPHP::StaticString s_MongoDriverExceptionDuplicateKeyException_className;
extern const HPHP::StaticString s_MongoDriverExceptionExecutionTimeoutException_className;
extern const HPHP::StaticString s_MongoDriverExceptionRuntimeException_className;
extern const HPHP::StaticString s_MongoDriverExceptionUnexpectedValueException_className;

class Utils
{
	public:
		static HPHP::ObjectData *CreateAndConstruct(const HPHP::StaticString classname, const HPHP::Variant &message, const HPHP::Variant &code);
		static bool splitNamespace(HPHP::String ns, char **db, char **col);
		static HPHP::Object throwInvalidArgumentException(char *errormessage);
		static HPHP::Object throwInvalidArgumentException(HPHP::String errormessage);
		static HPHP::Object throwRunTimeException(char *errormessage);
		static HPHP::Object throwUnexpectedValueException(char *errormessage);
		static HPHP::Object throwExceptionFromBsonError(bson_error_t *error);

		static HPHP::Object doExecuteBulkWrite(const HPHP::String ns, mongoc_client_t *client, int server_id, const HPHP::Object bulk, const mongoc_write_concern_t *write_concern);
		static HPHP::Object doExecuteCommand(const char *db, mongoc_client_t *client, int server_id, bson_t *command, HPHP::Variant readPreference);
		static HPHP::Object doExecuteQuery(const HPHP::String ns, mongoc_client_t *client, int server_id, HPHP::Object query, HPHP::Variant readPreference);
};

}
#endif
