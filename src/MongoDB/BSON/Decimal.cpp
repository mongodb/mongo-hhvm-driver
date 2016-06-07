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

#include "Decimal.h"

namespace HPHP {

const StaticString s_MongoBsonDecimal_className("MongoDB\\BSON\\Decimal");
Class* MongoDBBsonDecimalData::s_class = nullptr;
const StaticString MongoDBBsonDecimalData::s_className("MongoDBBsonDecimal");
IMPLEMENT_GET_CLASS(MongoDBBsonDecimalData);

const StaticString s_MongoDBBsonDecimal_decimal("decimal");

void HHVM_METHOD(MongoDBBsonDecimal, __construct, const String &decimal)
{
	MongoDBBsonDecimalData* data = Native::data<MongoDBBsonDecimalData>(this_);

	if (!bson_decimal128_from_string(decimal.c_str(), &data->m_decimal)) {
		throw MongoDriver::Utils::throwInvalidArgumentException("The argument (" + decimal + ") does not represent a valid decimal128 string");
	}
}

static String decimalAsString(MongoDBBsonDecimalData* data)
{
	String s;
	char *data_s;

	s = String(100, ReserveString);
	data_s = s.bufferSlice().data();
	bson_decimal128_to_string(&data->m_decimal, data_s);
	s.setSize(strlen(data_s));

	return s;
}

String HHVM_METHOD(MongoDBBsonDecimal, __toString)
{
	MongoDBBsonDecimalData* data = Native::data<MongoDBBsonDecimalData>(this_);

	return decimalAsString(data);
}

Array HHVM_METHOD(MongoDBBsonDecimal, __debugInfo)
{
	MongoDBBsonDecimalData* data = Native::data<MongoDBBsonDecimalData>(this_);
	Array retval = Array::Create();

	retval.add(s_MongoDBBsonDecimal_decimal, decimalAsString(data));

	return retval;
}

}
