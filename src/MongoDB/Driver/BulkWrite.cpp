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

#undef TRACE

#include "../../../bson.h"
#include "../../../utils.h"
#include "../../../mongodb.h"

#include "../BSON/ObjectID.h"
#include "WriteConcern.h"

#include "BulkWrite.h"

namespace HPHP {

Class* MongoDBDriverBulkWriteData::s_class = nullptr;
const StaticString MongoDBDriverBulkWriteData::s_className("MongoDBDriverBulkWrite");
IMPLEMENT_GET_CLASS(MongoDBDriverBulkWriteData);
const StaticString s_MongoDBDriverBulkWrite_ordered("ordered");
const StaticString s_MongoDBDriverBulkWrite_bypassDocumentValidation("bypassDocumentValidation");

#define BYPASS_UNSET -1

void HHVM_METHOD(MongoDBDriverBulkWrite, __construct, const Variant &bulkWriteOptions)
{
	MongoDBDriverBulkWriteData* data = Native::data<MongoDBDriverBulkWriteData>(this_);
	auto options = bulkWriteOptions.isNull() ? null_array : bulkWriteOptions.toArray();
	bool b_ordered = true;

	if (!options.isNull()) {
		if (options.exists(s_MongoDBDriverBulkWrite_ordered)) {
			b_ordered = options[s_MongoDBDriverBulkWrite_ordered].toInt64();
		}
	}

	data->m_bulk = mongoc_bulk_operation_new(b_ordered);
	data->m_num_ops = 0;
	data->m_ordered = b_ordered;
	data->m_bypass = BYPASS_UNSET;

	if (!options.isNull()) {
		if (options.exists(s_MongoDBDriverBulkWrite_bypassDocumentValidation)) {
			bool bypass = !!options[s_MongoDBDriverBulkWrite_bypassDocumentValidation].toBoolean();
			mongoc_bulk_operation_set_bypass_document_validation(data->m_bulk, bypass);
			data->m_bypass = bypass;
		}
	}
}

Variant HHVM_METHOD(MongoDBDriverBulkWrite, insert, const Variant &document)
{
	MongoDBDriverBulkWriteData* data = Native::data<MongoDBDriverBulkWriteData>(this_);
	bson_t *bson;

	VariantToBsonConverter converter(document, HIPPO_BSON_ADD_ID | HIPPO_BSON_RETURN_ID);
	bson = bson_new();
	converter.convert(bson);

	mongoc_bulk_operation_insert(data->m_bulk, bson);
	data->m_num_ops++;

	return Variant(converter.m_out);
}

const StaticString
	s_multi("multi"),
	s_upsert("upsert");


void HHVM_METHOD(MongoDBDriverBulkWrite, _update, int hasOperators, const Variant &query, const Variant &update, const Array &options)
{
	MongoDBDriverBulkWriteData* data = Native::data<MongoDBDriverBulkWriteData>(this_);
	bson_t *bquery;
	bson_t *bupdate;
	bson_t *boptions;
	bson_error_t error = { 0 };

	VariantToBsonConverter query_converter(query, HIPPO_BSON_NO_FLAGS);
	bquery = bson_new();
	query_converter.convert(bquery);

	VariantToBsonConverter update_converter(update, HIPPO_BSON_NO_FLAGS);
	bupdate = bson_new();
	update_converter.convert(bupdate);

	VariantToBsonConverter options_converter(options, HIPPO_BSON_NO_FLAGS);
	boptions = bson_new();
	options_converter.convert(boptions);

	if (hasOperators) {
		if (options.exists(s_multi) && options[s_multi].toBoolean()) {
			if (!mongoc_bulk_operation_update_many_with_opts(data->m_bulk, bquery, bupdate, boptions, &error)) {
				bson_clear(&bquery);
				bson_clear(&bupdate);
				bson_clear(&boptions);

				throw MongoDriver::Utils::throwExceptionFromBsonError(&error);
			}
		} else {
			if (!mongoc_bulk_operation_update_one_with_opts(data->m_bulk, bquery, bupdate, boptions, &error)) {
				bson_clear(&bquery);
				bson_clear(&bupdate);
				bson_clear(&boptions);

				throw MongoDriver::Utils::throwExceptionFromBsonError(&error);
			}
		}
	} else {
		if (!bson_validate(bupdate, (bson_validate_flags_t) (BSON_VALIDATE_DOT_KEYS | BSON_VALIDATE_DOLLAR_KEYS), NULL)) {
			bson_clear(&bquery);
			bson_clear(&bupdate);
			bson_clear(&boptions);

			throw MongoDriver::Utils::throwInvalidArgumentException("Replacement document may not contain \"$\" or \".\" in keys");
		}

		if (options.exists(s_multi) && options[s_multi].toBoolean()) {
			bson_clear(&bquery);
			bson_clear(&bupdate);
			bson_clear(&boptions);

			throw MongoDriver::Utils::throwInvalidArgumentException("Replacement document conflicts with true \"multi\" option");
		}

		if (!mongoc_bulk_operation_replace_one_with_opts(data->m_bulk, bquery, bupdate, boptions, &error)) {
			bson_clear(&bquery);
			bson_clear(&bupdate);
			bson_clear(&boptions);

			throw MongoDriver::Utils::throwExceptionFromBsonError(&error);
		}
	}

	data->m_num_ops++;

	bson_clear(&bquery);
	bson_clear(&bupdate);
	bson_clear(&boptions);
}

const StaticString
	s_limit("limit");

void HHVM_METHOD(MongoDBDriverBulkWrite, _delete, const Variant &query, const Array &options)
{
	MongoDBDriverBulkWriteData* data = Native::data<MongoDBDriverBulkWriteData>(this_);
	bson_t *bquery;
	bson_t *boptions;
	bson_error_t error = { 0 };

	VariantToBsonConverter query_converter(query, HIPPO_BSON_NO_FLAGS);
	bquery = bson_new();
	query_converter.convert(bquery);

	VariantToBsonConverter options_converter(options, HIPPO_BSON_NO_FLAGS);
	boptions = bson_new();
	options_converter.convert(boptions);

	if (options.exists(s_limit) && options[s_limit].toBoolean()) {
		if (!mongoc_bulk_operation_remove_one_with_opts(data->m_bulk, bquery, boptions, &error)) {
			bson_clear(&bquery);
			bson_clear(&boptions);

			throw MongoDriver::Utils::throwExceptionFromBsonError(&error);
		}
	} else {
		if (!mongoc_bulk_operation_remove_many_with_opts(data->m_bulk, bquery, boptions, &error)) {
			bson_clear(&bquery);
			bson_clear(&boptions);

			throw MongoDriver::Utils::throwExceptionFromBsonError(&error);
		}
	}

	data->m_num_ops++;

	bson_clear(&bquery);
	bson_clear(&boptions);
}

int64_t HHVM_METHOD(MongoDBDriverBulkWrite, count)
{
	MongoDBDriverBulkWriteData* data = Native::data<MongoDBDriverBulkWriteData>(this_);

	return data->m_num_ops;
}

const StaticString
	s_database("database"),
	s_collection("collection"),
	s_ordered("ordered"),
	s_bypassDocumentValidation("bypassDocumentValidation"),
	s_executed("executed"),
	s_server_id("server_id"),
	s_write_concern("write_concern");

Array HHVM_METHOD(MongoDBDriverBulkWrite, __debugInfo)
{
	MongoDBDriverBulkWriteData* data = Native::data<MongoDBDriverBulkWriteData>(this_);
	Array retval = Array::Create();

	if (data->m_database) {
		retval.set(s_database, data->m_database);
	} else {
		retval.set(s_database, Variant());
	}

	if (data->m_collection) {
		retval.set(s_collection, data->m_collection);
	} else {
		retval.set(s_collection, Variant());
	}

	retval.set(s_ordered, data->m_ordered);

	if (data->m_bypass != BYPASS_UNSET) {
		retval.set(s_bypassDocumentValidation, (bool) !!data->m_bypass);
	} else {
		retval.set(s_bypassDocumentValidation, Variant());
	}

	retval.set(s_executed, data->m_executed);
	retval.set(s_server_id, (int64_t) mongoc_bulk_operation_get_hint(data->m_bulk));

	if (mongoc_bulk_operation_get_write_concern(data->m_bulk)) {
		Array wc_retval = Array::Create();
		mongodb_driver_add_write_concern_debug(mongoc_bulk_operation_get_write_concern(data->m_bulk), &wc_retval);
		retval.set(s_write_concern, wc_retval);
	} else {
		retval.set(s_write_concern, Variant());
	}

	return retval;
}

}
