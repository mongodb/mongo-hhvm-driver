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

#include "bson.h"
#include <iostream>

extern "C" {
#include "libbson/src/bson/bson.h"
#include "libmongoc/src/mongoc/mongoc.h"
}

namespace HPHP {

	VariantToBsonConverter::VariantToBsonConverter(const Variant& document)
	{
		m_document = document;
		m_level = 0;
	}

	bson_t *VariantToBsonConverter::convert(bson_t *bson)
	{
		m_output = bson;

		convert(m_document);

		return m_output;
	}

	void VariantToBsonConverter::convert(Variant v)
	{
		std::cout << "convert Variant\n";

		if (v.isObject()) {
//			convertPart(v.toObject()); 
		} else if (v.isArray()) {
			convert(v.toArray()); 
		} else {
			std::cout << "convert *unimplemented*: " << getDataTypeString(v.getType()).c_str() << "\n";
		}
	}

	void VariantToBsonConverter::convertPart(const char *key, Variant v)
	{
		switch (v.getType()) {
			case KindOfUninit:
			case KindOfNull:
				convertPart(key);
				break;
			case KindOfBoolean:
				convertPart(key, v.toBoolean());
				break;
			case KindOfInt64:
				convertPart(key, v.toInt64());
				break;
			case KindOfDouble:
				convertPart(key, v.toDouble());
				break;
			case KindOfStaticString:
			case KindOfString:
				convertPart(key, v.toString());
				break;
			case KindOfArray:
				convertPart(key, v.toArray());
				break;
			case KindOfObject:
				convertPart(key, v.toObject());
				break;
			default:
				break;
		}
	}

	void VariantToBsonConverter::convertPart(const char *key)
	{
		std::cout << "null\n";
		bson_append_null(m_output, key, -1);
	};
	
	void VariantToBsonConverter::convertPart(const char *key, bool v)
	{
		std::cout << "bool\n";
		bson_append_bool(m_output, key, -1, v);
	};

	void VariantToBsonConverter::convertPart(const char *key, int64_t v)
	{
		std::cout << "int64\n";
		bson_append_int64(m_output, key, -1, v);
	};

	void VariantToBsonConverter::convertPart(const char *key, double v)
	{
		std::cout << "double\n";
		bson_append_double(m_output, key, -1, v);
	};

	void VariantToBsonConverter::convertPart(const char *key, String v)
	{
		std::cout << "utf8: " << v.c_str() << "\n";
		bson_append_utf8(m_output, key, -1, v.c_str(), v.size());
	}

	void VariantToBsonConverter::convertPart(const char *key, Array v) { std::cout << "x\n"; };
	void VariantToBsonConverter::convertPart(const char *key, Object v) { std::cout << "x\n"; };

	void VariantToBsonConverter::convert(Array a)
	{
		std::cout << "convert Top Level Array\n";

		for (ArrayIter iter(a); iter; ++iter) {
			Variant key(iter.first());
			const Variant& data(iter.secondRef());

			convertPart(key.toString().c_str(), data);
		}
	}

	void VariantToBsonConverter::convert(Object o)
	{
		std::cout << "convert Top Level Object\n";
/*
		for (ArrayIter iter(a); iter; ++iter) {
			Variant key(iter.first());
			const Variant& data(iter.secondRef());

			convertPart(key.toString().c_str(), data);
		}
*/
	}
}
