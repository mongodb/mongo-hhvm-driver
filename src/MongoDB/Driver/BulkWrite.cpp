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

#include "../BSON/ObjectId.h"

#include "BulkWrite.h"

namespace HPHP {

Class* MongoDBDriverBulkWriteData::s_class = nullptr;
const StaticString MongoDBDriverBulkWriteData::s_className("MongoDBDriverBulkWrite");
IMPLEMENT_GET_CLASS(MongoDBDriverBulkWriteData);

void HHVM_METHOD(MongoDBDriverBulkWrite, __construct, const Variant &ordered)
{
	MongoDBDriverBulkWriteData* data = Native::data<MongoDBDriverBulkWriteData>(this_);
	bool b_ordered = ordered.toInt64();

	std::cout << "ordered: " << b_ordered << "\n";
	data->m_bulk = mongoc_bulk_operation_new(b_ordered);
}

Object HHVM_METHOD(MongoDBDriverBulkWrite, insert, const Variant &document)
{
	MongoDBDriverBulkWriteData* data = Native::data<MongoDBDriverBulkWriteData>(this_);
	bson_t *bson;

	VariantToBsonConverter converter(document, HIPPO_BSON_ADD_ID | HIPPO_BSON_RETURN_ID);
	bson = bson_new();
	converter.convert(bson);

	mongoc_bulk_operation_insert(data->m_bulk, bson);

	/* Should always trigger, because we set HIPPO_BSON_RETURN_ID. We would
	 * like to do this only when the return value is used, but unlike PHP, this
	 * is not detectable in HHVM */
	if (converter.m_out) {
		bson_iter_t iter;

		if (bson_iter_init_find(&iter, converter.m_out, "_id")) {
			static Class* c_objectId;
			c_objectId = Unit::lookupClass(s_MongoBsonObjectId_className.get());
			assert(c_objectId);
			ObjectData* obj = ObjectData::newInstance(c_objectId);

			MongoDBBsonObjectIdData* obj_data = Native::data<MongoDBBsonObjectIdData>(obj);
			bson_oid_copy(bson_iter_oid(&iter), &obj_data->m_oid);

			bson_clear(&converter.m_out);

			return obj;
		}

		bson_clear(&converter.m_out);
	}

	return NULL;
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
