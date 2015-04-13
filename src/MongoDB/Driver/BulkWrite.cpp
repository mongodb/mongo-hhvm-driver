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

#include "../../../bson.h"
#include "../../../utils.h"
#include "../../../mongodb.h"

#include "BulkWrite.h"

namespace HPHP {

Class* MongoDBDriverBulkWriteData::s_class = nullptr;
const StaticString MongoDBDriverBulkWriteData::s_className("MongoDBDriverBulkWrite");
IMPLEMENT_GET_CLASS(MongoDBDriverBulkWriteData);

void HHVM_METHOD(MongoDBDriverBulkWrite, __construct, bool ordered)
{
}

Object HHVM_METHOD(MongoDBDriverBulkWrite, insert, const Variant &document)
{
}

Object HHVM_METHOD(MongoDBDriverBulkWrite, update, const Variant &query, const Variant &newObj, const Array &updateOptions)
{
}

Object HHVM_METHOD(MongoDBDriverBulkWrite, delete, const Variant &qyert, const Array &deleteOptions)
{
}

int64_t HHVM_METHOD(MongoDBDriverBulkWrite, count)
{
}

Array HHVM_METHOD(MongoDBDriverBulkWrite, __debugInfo)
{
}

}
