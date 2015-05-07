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

#include "hphp/runtime/base/base-includes.h"
#include "hphp/runtime/base/array-init.h"
#include "hphp/runtime/base/execution-context.h"

#include "utils.h"

namespace MongoDriver
{

bool Utils::splitNamespace(HPHP::String ns, char **db, char **col)
{
	const char *input = ns.c_str();
	const char *dot = strchr(input, '.');                                      

	if (!dot) {                                                                         
		return false;        
	}                                                                            

	if (col) {                                                                        
		*col = strdup(input + (dot - input) + 1);          
	}            
	if (db) {    
		*db = strndup(input, dot - input);
	}                       

	return true;  
}

HPHP::ObjectData *Utils::CreateAndConstruct(HPHP::StaticString classname, const HPHP::Variant &message, const HPHP::Variant &code)
{
	static HPHP::Class* c_class;

	c_class = HPHP::Unit::lookupClass(classname.get());
	assert(c_class);
	HPHP::ObjectData *inst = HPHP::ObjectData::newInstance(c_class);

	HPHP::TypedValue ret;
	{
		HPHP::CountableHelper cnt(inst);
		HPHP::g_context->invokeFunc(&ret, c_class->getCtor(), HPHP::make_packed_array(message, code), inst);
	}
	HPHP::tvRefcountedDecRef(&ret);

	return inst;
}

const HPHP::StaticString s_MongoDriverExceptionAuthenticationException_className("MongoDB\\Driver\\Exception\\AuthenticationException");
const HPHP::StaticString s_MongoDriverExceptionConnectionException_className("MongoDB\\Driver\\Exception\\ConnectionException");
const HPHP::StaticString s_MongoDriverExceptionConnectionTimeoutException_className("MongoDB\\Driver\\Exception\\ConnectionTimeoutException");
const HPHP::StaticString s_MongoDriverExceptionDuplicateKeyException_className("MongoDB\\Driver\\Exception\\DuplicateKeyException");
const HPHP::StaticString s_MongoDriverExceptionExecutionTimeoutException_className("MongoDB\\Driver\\Exception\\ExecutionTimeoutException");
const HPHP::StaticString s_MongoDriverExceptionRuntimeException_className("MongoDB\\Driver\\Exception\\RuntimeException");

HPHP::Object Utils::throwExceptionFromBsonError(bson_error_t *error)
{
	switch (error->code) {
		case 50: /* ExceededTimeLimit */
			return Utils::CreateAndConstruct(s_MongoDriverExceptionExecutionTimeoutException_className, HPHP::Variant(error->message), HPHP::Variant((uint64_t) error->code));
		case MONGOC_ERROR_STREAM_SOCKET:
			return Utils::CreateAndConstruct(s_MongoDriverExceptionConnectionTimeoutException_className, HPHP::Variant(error->message), HPHP::Variant((uint64_t) error->code));
		case 11000: /* DuplicateKey */
			return Utils::CreateAndConstruct(s_MongoDriverExceptionDuplicateKeyException_className, HPHP::Variant(error->message), HPHP::Variant((uint64_t) error->code));
		case MONGOC_ERROR_CLIENT_AUTHENTICATE:
			return Utils::CreateAndConstruct(s_MongoDriverExceptionAuthenticationException_className, HPHP::Variant(error->message), HPHP::Variant((uint64_t) error->code));

		case MONGOC_ERROR_STREAM_INVALID_TYPE:
		case MONGOC_ERROR_STREAM_INVALID_STATE:
		case MONGOC_ERROR_STREAM_NAME_RESOLUTION:
		case MONGOC_ERROR_STREAM_CONNECT:
		case MONGOC_ERROR_STREAM_NOT_ESTABLISHED:
			return Utils::CreateAndConstruct(s_MongoDriverExceptionConnectionException_className, HPHP::Variant(error->message), HPHP::Variant((uint64_t) error->code));
		case MONGOC_ERROR_CLIENT_NOT_READY:
		case MONGOC_ERROR_CLIENT_TOO_BIG:
		case MONGOC_ERROR_CLIENT_TOO_SMALL:
		case MONGOC_ERROR_CLIENT_GETNONCE:
		case MONGOC_ERROR_CLIENT_NO_ACCEPTABLE_PEER:
		case MONGOC_ERROR_CLIENT_IN_EXHAUST:
		case MONGOC_ERROR_PROTOCOL_INVALID_REPLY:
		case MONGOC_ERROR_PROTOCOL_BAD_WIRE_VERSION:
		case MONGOC_ERROR_CURSOR_INVALID_CURSOR:
		case MONGOC_ERROR_QUERY_FAILURE:
		/*case MONGOC_ERROR_PROTOCOL_ERROR:*/
		case MONGOC_ERROR_BSON_INVALID:
		case MONGOC_ERROR_MATCHER_INVALID:
		case MONGOC_ERROR_NAMESPACE_INVALID:
		case MONGOC_ERROR_COMMAND_INVALID_ARG:
		case MONGOC_ERROR_COLLECTION_INSERT_FAILED:
		case MONGOC_ERROR_GRIDFS_INVALID_FILENAME:
		case MONGOC_ERROR_QUERY_COMMAND_NOT_FOUND:
		case MONGOC_ERROR_QUERY_NOT_TAILABLE:
			return Utils::CreateAndConstruct(s_MongoDriverExceptionRuntimeException_className, HPHP::Variant(error->message), HPHP::Variant((uint64_t) error->code));
	}
	switch (error->domain) {
		case MONGOC_ERROR_CLIENT:
		case MONGOC_ERROR_STREAM:
		case MONGOC_ERROR_PROTOCOL:
		case MONGOC_ERROR_CURSOR:
		case MONGOC_ERROR_QUERY:
		case MONGOC_ERROR_INSERT:
		case MONGOC_ERROR_SASL:
		case MONGOC_ERROR_BSON:
		case MONGOC_ERROR_MATCHER:
		case MONGOC_ERROR_NAMESPACE:
		case MONGOC_ERROR_COMMAND:
		case MONGOC_ERROR_COLLECTION:
		case MONGOC_ERROR_GRIDFS:
			/* FIXME: We don't have the Exceptions mocked yet.. */
#if 0
			return phongo_ce_mongo_connection_exception;
#endif
		default:
			return HPHP::Object(HPHP::SystemLib::AllocRuntimeExceptionObject(error->message));
	}
}

}
