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
#ifndef __MONGODB_BSON_BINARY_H__
#define __MONGODB_BSON_BINARY_H__

extern "C" {
#include "../../../libbson/src/bson/bson.h"
}

namespace HPHP {

extern const StaticString s_MongoBsonBinary_className;
extern const StaticString s_MongoBsonBinary_data;
extern const StaticString s_MongoBsonBinary_type;

ObjectData* createMongoBsonBinaryObject(const uint8_t *v_binary, size_t v_binary_len, bson_subtype_t v_subtype);

Array HHVM_METHOD(MongoDBBsonBinary, __debugInfo);
}
#endif

