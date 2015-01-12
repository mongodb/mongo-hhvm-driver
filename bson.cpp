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

int VariantToBsonConverter::_isPackedArray(const Array &a)
{
	int idx = 0, key_value = 0;

	for (ArrayIter iter(a); iter; ++iter) {
		Variant key(iter.first());

		if (!key.isInteger()) {
			return false;
		}

		key_value = key.toInt32(); 
		std::cout << idx << "key: " << key_value << "\n";

		if (idx != key_value) {
			return false;
		}

		idx++;
	}
	return true;
}

VariantToBsonConverter::VariantToBsonConverter(const Variant& document)
{
	m_document = document;
	m_level = 0;
}

void VariantToBsonConverter::convert(bson_t *bson)
{
	return convert(bson, m_document);
}

void VariantToBsonConverter::convert(bson_t *bson, Variant v)
{
	std::cout << "convert Variant\n";

	if (v.isObject()) {
//		convert(bson, v.toObject());
	} else if (v.isArray()) {
		convert(bson, v.toArray()); 
	} else {
		std::cout << "convert *unimplemented*: " << getDataTypeString(v.getType()).c_str() << "\n";
	}
}

void VariantToBsonConverter::convertPart(bson_t *bson, const char *key, Variant v)
{
	switch (v.getType()) {
		case KindOfUninit:
		case KindOfNull:
			convertPart(bson, key);
			break;
		case KindOfBoolean:
			convertPart(bson, key, v.toBoolean());
			break;
		case KindOfInt64:
			convertPart(bson, key, v.toInt64());
			break;
		case KindOfDouble:
			convertPart(bson, key, v.toDouble());
			break;
		case KindOfStaticString:
		case KindOfString:
			convertPart(bson, key, v.toString());
			break;
		case KindOfArray:
			convertPart(bson, key, v.toArray());
			break;
		case KindOfObject:
			convertPart(bson, key, v.toObject());
			break;
		default:
			break;
	}
}

void VariantToBsonConverter::convertPart(bson_t *bson, const char *key)
{
	std::cout << "null\n";
	bson_append_null(bson, key, -1);
};

void VariantToBsonConverter::convertPart(bson_t *bson, const char *key, bool v)
{
	std::cout << "bool\n";
	bson_append_bool(bson, key, -1, v);
};

void VariantToBsonConverter::convertPart(bson_t *bson, const char *key, int64_t v)
{
	std::cout << "int64\n";
	bson_append_int64(bson, key, -1, v);
};

void VariantToBsonConverter::convertPart(bson_t *bson, const char *key, double v)
{
	std::cout << "double\n";
	bson_append_double(bson, key, -1, v);
};

void VariantToBsonConverter::convertPart(bson_t *bson, const char *key, String v)
{
	std::cout << "key: " << key << ", utf8: " << v.c_str() << "\n";
	bson_append_utf8(bson, key, -1, v.c_str(), v.size());
}

char *VariantToBsonConverter::_getUnmangledPropertyName(String key)
{
	const char *ckey = key.c_str();

	if (ckey[0] == '\0' && key.length()) {
		const char *cls = ckey + 1;
		if (*cls == '*') { // protected
			return strdup(ckey + 3);
		} else {
			int l = strlen(cls);
			return strdup(cls + l + 1);
		}
	} else {
		return strdup(ckey);
	}
}

void VariantToBsonConverter::convertPart(bson_t *bson, const char *key, Array v)
{
	bson_t child;
	int unmangle = 0;

	std::cout << "array\n";

	if (_isPackedArray(v)) {
		bson_append_array_begin(bson, key, -1, &child);
	} else {
		unmangle = 1;
		bson_append_document_begin(bson, key, -1, &child);
	}

	for (ArrayIter iter(v); iter; ++iter) {
		Variant key(iter.first());
		const Variant& data(iter.secondRef());

		if (unmangle) {
			const char *unmangledName;

			unmangledName = _getUnmangledPropertyName(key.toString());
			convertPart(&child, unmangledName, data);
			free((void*) unmangledName);
		} else {
			convertPart(&child, key.toString().c_str(), data);
		}
	}

	if (_isPackedArray(v)) {
		bson_append_array_end(bson, &child);
	} else {
		bson_append_document_end(bson, &child);
	}
}

void VariantToBsonConverter::convertPart(bson_t *bson, const char *key, Object v)
{
	std::cout << "object coverted to ";

	convertPart(bson, key, v.toArray());
}

void VariantToBsonConverter::convert(bson_t *bson, Array a)
{
	std::cout << "convert Top Level Array\n";

	for (ArrayIter iter(a); iter; ++iter) {
		Variant key(iter.first());
		const Variant& data(iter.secondRef());

		convertPart(bson, key.toString().c_str(), data);
	}
}

void VariantToBsonConverter::convert(bson_t *bson, Object o)
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

} /* namespace HPHP */
