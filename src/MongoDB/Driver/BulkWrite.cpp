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

/* We need to access the internal bulk structure to access the bulk op count */
#define MONGOC_I_AM_A_DRIVER
#define delete not_delete
#include "../../../libmongoc/src/mongoc/mongoc-bulk-operation-private.h"
#undef delete
#undef MONGOC_I_AM_A_DRIVER

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

void HHVM_METHOD(MongoDBDriverBulkWrite, update, const Variant &query, const Variant &update, const Variant &updateOptions)
{
	MongoDBDriverBulkWriteData* data = Native::data<MongoDBDriverBulkWriteData>(this_);
	bson_t *bquery;
	bson_t *bupdate;
	auto options = updateOptions.isNull() ? null_array : updateOptions.toArray();
	int flags = MONGOC_UPDATE_NONE;

	VariantToBsonConverter query_converter(query, HIPPO_BSON_NO_FLAGS);
	bquery = bson_new();
	query_converter.convert(bquery);

	VariantToBsonConverter update_converter(update, HIPPO_BSON_NO_FLAGS);
	bupdate = bson_new();
	update_converter.convert(bupdate);

	if (!updateOptions.isNull()) {
		if (options.exists(String("multi"))) {
			Variant v_multi = options[String("multi")];
			bool multi = v_multi.toBoolean();

			if (multi) {
				flags |= MONGOC_UPDATE_MULTI_UPDATE;
			}
		}

		if (options.exists(String("upsert"))) {
			Variant v_upsert = options[String("upsert")];
			bool upsert = v_upsert.toBoolean();

			if (upsert) {
				flags |= MONGOC_UPDATE_UPSERT;
			}
		}
	}

	std::cout << "flags: " << flags << "\n";

	if (flags & MONGOC_UPDATE_MULTI_UPDATE) {
		mongoc_bulk_operation_update(data->m_bulk, bquery, bupdate, !!(flags & MONGOC_UPDATE_UPSERT));
	} else {
		bson_iter_t iter;
		bool   replaced = 0;

		if (bson_iter_init(&iter, bupdate)) {
			while (bson_iter_next(&iter)) {
				if (!strchr(bson_iter_key (&iter), '$')) {
					mongoc_bulk_operation_replace_one(data->m_bulk, bquery, bupdate, !!(flags & MONGOC_UPDATE_UPSERT));
					replaced = 1;
					break;
				}
			}
		}

		if (!replaced) {
			mongoc_bulk_operation_update_one(data->m_bulk, bquery, bupdate, !!(flags & MONGOC_UPDATE_UPSERT));
		}
	}

	bson_clear(&bquery);
	bson_clear(&bupdate);
}

void HHVM_METHOD(MongoDBDriverBulkWrite, delete, const Variant &query, const Variant &deleteOptions)
{
	MongoDBDriverBulkWriteData* data = Native::data<MongoDBDriverBulkWriteData>(this_);
	bson_t *bquery;
	auto options = deleteOptions.isNull() ? null_array : deleteOptions.toArray();

	VariantToBsonConverter query_converter(query, HIPPO_BSON_NO_FLAGS);
	bquery = bson_new();
	query_converter.convert(bquery);

	if ((!deleteOptions.isNull()) && (options.exists(String("limit")))) {
		Variant v_limit = options[String("limit")];
		bool limit = v_limit.toBoolean();

		if (limit) {
			mongoc_bulk_operation_remove_one(data->m_bulk, bquery);
		} else {
			mongoc_bulk_operation_remove(data->m_bulk, bquery);
		}
	} else {
		mongoc_bulk_operation_remove(data->m_bulk, bquery);
	}

	bson_clear(&bquery);
}

int64_t HHVM_METHOD(MongoDBDriverBulkWrite, count)
{
	MongoDBDriverBulkWriteData* data = Native::data<MongoDBDriverBulkWriteData>(this_);

	return data->m_bulk->commands.len;
}

const StaticString
	s_database("database"),
	s_collection("collection"),
	s_ordered("ordered"),
	s_executed("executed"),
	s_server_id("server_id"),
	s_write_concern("write_concern");


Array HHVM_METHOD(MongoDBDriverBulkWrite, __debugInfo)
{
	MongoDBDriverBulkWriteData* data = Native::data<MongoDBDriverBulkWriteData>(this_);
	Array retval = Array::Create();

	if (data->m_bulk->database) {
		retval.set(s_database, data->m_bulk->database);
	} else {
		retval.set(s_database, Variant());
	}

	if (data->m_bulk->collection) {
		retval.set(s_collection, data->m_bulk->collection);
	} else {
		retval.set(s_collection, Variant());
	}

	retval.set(s_ordered, data->m_bulk->ordered);
	retval.set(s_executed, data->m_bulk->executed);
	retval.set(s_server_id, (int64_t) data->m_bulk->hint);

	if (data->m_bulk->write_concern) {
		retval.set(s_write_concern, Variant(true));
	} else {
		retval.set(s_write_concern, Variant());
	}

	return retval;
}

}
