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

#include "../../../mongodb.h"
#include "../../../utils.h"

#include "Decimal128.h"

namespace HPHP {

const StaticString s_MongoBsonDecimal128_className("MongoDB\\BSON\\Decimal128");
const StaticString s_MongoBsonDecimal128_dec("dec");
Class* MongoDBBsonDecimal128Data::s_class = nullptr;
const StaticString MongoDBBsonDecimal128Data::s_className("MongoDBBsonDecimal128");
IMPLEMENT_GET_CLASS(MongoDBBsonDecimal128Data);

const StaticString s_MongoDBBsonDecimal128_decimal("decimal");

static String decimalAsString(MongoDBBsonDecimal128Data* data)
{
	String s;
	char *data_s;

	s = String(100, ReserveString);
	data_s = s.bufferSlice().data();
	bson_decimal128_to_string(&data->m_decimal, data_s);
	s.setSize(strlen(data_s));

	return s;
}

void HHVM_METHOD(MongoDBBsonDecimal128, __construct, const String &decimal)
{
	MongoDBBsonDecimal128Data* data = Native::data<MongoDBBsonDecimal128Data>(this_);

	if (!bson_decimal128_from_string(decimal.c_str(), &data->m_decimal)) {
		throw MongoDriver::Utils::throwInvalidArgumentException("Error parsing Decimal128 string: " + decimal);
	}

	this_->o_set(s_MongoBsonDecimal128_dec, Variant(decimalAsString(data)), s_MongoBsonDecimal128_className);
}

String HHVM_METHOD(MongoDBBsonDecimal128, __toString)
{
	MongoDBBsonDecimal128Data* data = Native::data<MongoDBBsonDecimal128Data>(this_);

	return decimalAsString(data);
}

Array HHVM_METHOD(MongoDBBsonDecimal128, __debugInfo)
{
	MongoDBBsonDecimal128Data* data = Native::data<MongoDBBsonDecimal128Data>(this_);
	Array retval = Array::Create();

	retval.add(s_MongoDBBsonDecimal128_decimal, decimalAsString(data));

	return retval;
}

}
