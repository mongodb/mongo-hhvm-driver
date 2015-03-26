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

#include "src/MongoDB/BSON/Binary.h"

extern "C" {
#include "libbson/src/bson/bson.h"
#include "libmongoc/src/mongoc/mongoc.h"
}

namespace HPHP {
/* {{{ HHVM → BSON */
int VariantToBsonConverter::_isPackedArray(const Array &a)
{
	int idx = 0, key_value = 0;

	for (ArrayIter iter(a); iter; ++iter) {
		Variant key(iter.first());

		if (!key.isInteger()) {
			return false;
		}

		key_value = key.toInt32(); 

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
	if (v.isObject()) {
		convert(bson, v.toObject());
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
			convertPart(bson, key, v.toArray(), true);
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
	bson_append_null(bson, key, -1);
};

void VariantToBsonConverter::convertPart(bson_t *bson, const char *key, bool v)
{
	bson_append_bool(bson, key, -1, v);
};

void VariantToBsonConverter::convertPart(bson_t *bson, const char *key, int64_t v)
{
	if (v > INT_MAX || v < INT_MIN) {
		bson_append_int64(bson, key, -1, v);
	} else {
		bson_append_int32(bson, key, -1, (int32_t) v);
	}
};

void VariantToBsonConverter::convertPart(bson_t *bson, const char *key, double v)
{
	bson_append_double(bson, key, -1, v);
};

void VariantToBsonConverter::convertPart(bson_t *bson, const char *key, String v)
{
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

void VariantToBsonConverter::convertPart(bson_t *bson, const char *key, Array v, bool wrap)
{
	bson_t child;
	int unmangle = 0;

	if (_isPackedArray(v)) {
		if (wrap) {
			bson_append_array_begin(bson, key, -1, &child);
		}
	} else {
		unmangle = 1;
		if (wrap) {
			bson_append_document_begin(bson, key, -1, &child);
		}
	}

	for (ArrayIter iter(v); iter; ++iter) {
		Variant key(iter.first());
		const Variant& data(iter.secondRef());

		if (unmangle) {
			const char *unmangledName;

			unmangledName = _getUnmangledPropertyName(key.toString());
			convertPart(wrap ? &child : bson, unmangledName, data);
			free((void*) unmangledName);
		} else {
			convertPart(wrap ? &child : bson, key.toString().c_str(), data);
		}
	}

	if (wrap) {
		if (_isPackedArray(v)) {
			bson_append_array_end(bson, &child);
		} else {
			bson_append_document_end(bson, &child);
		}
	}
}

/* {{{ Serialization of types */
const StaticString s_MongoDriverBsonType_className("MongoDB\\BSON\\Type");

/* {{{ MongoDriver\BSON\Binary */
void VariantToBsonConverter::_convertBinary(bson_t *bson, const char *key, Object v)
{
	String data = v.o_get(s_MongoBsonBinary_data, false, s_MongoBsonBinary_className);
	int64_t subType = v.o_get(s_MongoBsonBinary_subType, false, s_MongoBsonBinary_className).toInt64();

	bson_append_binary(bson, key, -1, (bson_subtype_t) subType, (const unsigned char*) data.c_str(), data.length());
}
/* }}} */

/* {{{ MongoDriver\BSON\Regex */
const StaticString s_MongoDriverBsonRegex_className("MongoDB\\BSON\\Regex");
const StaticString s_MongoDriverBsonRegex_pattern("pattern");
const StaticString s_MongoDriverBsonRegex_flags("flags");

void VariantToBsonConverter::_convertRegex(bson_t *bson, const char *key, Object v)
{
	String regex = v.o_get(s_MongoDriverBsonRegex_pattern, false, s_MongoDriverBsonRegex_className);
	String flags = v.o_get(s_MongoDriverBsonRegex_flags, false, s_MongoDriverBsonRegex_className);

	bson_append_regex(bson, key, -1, regex.c_str(), flags.c_str());
}
/* }}} */

/* }}} */

void VariantToBsonConverter::convertPart(bson_t *bson, const char *key, Object v)
{
	if (v.instanceof(s_MongoDriverBsonType_className)) {
		if (v.instanceof(s_MongoBsonBinary_className)) {
			_convertBinary(bson, key, v);
		}
		if (v.instanceof(s_MongoDriverBsonRegex_className)) {
			_convertRegex(bson, key, v);
		}
	} else {
		convertPart(bson, key, v.toArray());
	}
}

void VariantToBsonConverter::convert(bson_t *bson, Array a)
{
	convertPart(bson, NULL, a, false);
}

void VariantToBsonConverter::convert(bson_t *bson, Object o)
{
	convert(bson, o.toArray());
}
/* }}} */

/* {{{ BSON → HHVM */
BsonToVariantConverter::BsonToVariantConverter(const unsigned char *data, int data_len)
{
	m_reader = bson_reader_new_from_data(data, data_len);
}

/* {{{ Visitors */
void hippo_bson_visit_corrupt(const bson_iter_t *iter __attribute__((unused)), void *data)
{
	std::cout << "converting corrupt\n";
}

bool hippo_bson_visit_double(const bson_iter_t *iter __attribute__((unused)), const char *key, double v_double, void *data)
{
	hippo_bson_state *state = (hippo_bson_state*) data;

	std::cout << "converting double: " << key << ": " << v_double << "\n";
	state->zchild->add(String(key), Variant(v_double));
	return false;
}

bool hippo_bson_visit_utf8(const bson_iter_t *iter __attribute__((unused)), const char *key, size_t v_utf8_len, const char *v_utf8, void *data)
{
	hippo_bson_state *state = (hippo_bson_state*) data;

	std::cout << "converting utf8: " << key << ": " << v_utf8 << "\n";
	state->zchild->add(String(key), Variant(v_utf8));
	return false;
}

bool hippo_bson_visit_document(const bson_iter_t *iter __attribute__((unused)), const char *key, const bson_t *v_document, void *data)
{
	std::cout << "converting document\n";
	return false;
}

bool hippo_bson_visit_array(const bson_iter_t *iter __attribute__((unused)), const char *key, const bson_t *v_array, void *data)
{
	std::cout << "converting array\n";
	return false;
}

bool hippo_bson_visit_binary(const bson_iter_t *iter __attribute__((unused)), const char *key, bson_subtype_t v_subtype, size_t v_binary_len, const uint8_t *v_binary, void *data)
{
	hippo_bson_state *state = (hippo_bson_state*) data;
	static Class* c_binary;
	String s;
	unsigned char *data_s;

	std::cout << "converting binary\n";

	s = String(v_binary_len, ReserveString);
	data_s = (unsigned char*) s.bufferSlice().ptr;
	memcpy(data_s, v_binary, v_binary_len);
	s.setSize(v_binary_len);

	c_binary = Unit::lookupClass(s_MongoBsonBinary_className.get());
	assert(c_binary);
	ObjectData* obj = ObjectData::newInstance(c_binary);

	obj->o_set(s_MongoBsonBinary_data, s, s_MongoBsonBinary_className.get());
	obj->o_set(s_MongoBsonBinary_subType, Variant(v_subtype), s_MongoBsonBinary_className.get());
	
	state->zchild->add(String(key), Variant(obj));

	return false;
}

bool hippo_bson_visit_oid(const bson_iter_t *iter __attribute__((unused)), const char *key, const bson_oid_t *v_oid, void *data)
{
	std::cout << "converting oid\n";
	return false;
}

bool hippo_bson_visit_bool(const bson_iter_t *iter __attribute__((unused)), const char *key, bool v_bool, void *data)
{
	hippo_bson_state *state = (hippo_bson_state*) data;

	std::cout << "converting bool: " << key << ": " << v_bool << "\n";
	state->zchild->add(String(key), Variant(v_bool));
	return false;
}

bool hippo_bson_visit_date_time(const bson_iter_t *iter __attribute__((unused)), const char *key, int64_t msec_since_epoch, void *data)
{
	std::cout << "converting date_time\n";
	return false;
}

bool hippo_bson_visit_null(const bson_iter_t *iter __attribute__((unused)), const char *key, void *data)
{
	hippo_bson_state *state = (hippo_bson_state*) data;

	std::cout << "converting null: " << key << "\n";
	state->zchild->add(String(key), Variant(Variant::NullInit()));
	return false;
}

bool hippo_bson_visit_regex(const bson_iter_t *iter __attribute__((unused)), const char *key, const char *v_regex, const char *v_options, void *data)
{
	std::cout << "converting regex\n";
	return false;
}

bool hippo_bson_visit_code(const bson_iter_t *iter __attribute__((unused)), const char *key, size_t v_code_len, const char *v_code, void *data)
{
	std::cout << "converting code\n";
	return false;
}

bool hippo_bson_visit_codewscope(const bson_iter_t *iter __attribute__((unused)), const char *key, size_t v_code_len, const char *v_code, const bson_t *v_scope, void *data)
{
	std::cout << "converting codewscope\n";
	return false;
}

bool hippo_bson_visit_int32(const bson_iter_t *iter __attribute__((unused)), const char *key, int32_t v_int32, void *data)
{
	hippo_bson_state *state = (hippo_bson_state*) data;

	std::cout << "converting int32: " << key << ": " << v_int32 << "\n";
	state->zchild->add(String(key), Variant(v_int32));
	return false;
}

bool hippo_bson_visit_timestamp(const bson_iter_t *iter __attribute__((unused)), const char *key, uint32_t v_timestamp, uint32_t v_increment, void *data)
{
	std::cout << "converting timestamp\n";
	return false;
}

bool hippo_bson_visit_int64(const bson_iter_t *iter __attribute__((unused)), const char *key, int64_t v_int64, void *data)
{
	hippo_bson_state *state = (hippo_bson_state*) data;

	std::cout << "converting int64: " << key << ": " << v_int64 << "\n";
	state->zchild->add(String(key), Variant(v_int64));
	return false;
}

bool hippo_bson_visit_maxkey(const bson_iter_t *iter __attribute__((unused)), const char *key, void *data)
{
	std::cout << "converting maxkey\n";
	return false;
}

bool hippo_bson_visit_minkey(const bson_iter_t *iter __attribute__((unused)), const char *key, void *data)
{
	std::cout << "converting minkey\n";
	return false;
}


static const bson_visitor_t hippo_bson_visitors = {
	NULL /* hippo_phongo_bson_visit_before*/,
	NULL /*hippo_phongo_bson_visit_after*/,
	hippo_bson_visit_corrupt,
	hippo_bson_visit_double,
	hippo_bson_visit_utf8,
	hippo_bson_visit_document,
	hippo_bson_visit_array,
	hippo_bson_visit_binary,
	NULL /*hippo_bson_visit_undefined*/,
	hippo_bson_visit_oid,
	hippo_bson_visit_bool,
	hippo_bson_visit_date_time,
	hippo_bson_visit_null,
	hippo_bson_visit_regex,
	NULL /*hippo_bson_visit_dbpointer*/,
	hippo_bson_visit_code,
	NULL /*hippo_bson_visit_symbol*/,
	hippo_bson_visit_codewscope,
	hippo_bson_visit_int32,
	hippo_bson_visit_timestamp,
	hippo_bson_visit_int64,
	hippo_bson_visit_maxkey,
	hippo_bson_visit_minkey,
	{ NULL }
};
/* }}} */


bool BsonToVariantConverter::convert(Variant *v)
{
	bson_iter_t   iter;
	bool          eof = false;
	const bson_t *b;
	hippo_bson_state state;

	state.zchild = NULL;

	if (!(b = bson_reader_read(m_reader, &eof))) {
//		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not read document from reader");
		return false;
	}

	do {
		if (!bson_iter_init(&iter, b)) {
			bson_reader_destroy(m_reader);
			return false;
		}

		state.zchild = new Array;
		bson_iter_visit_all(&iter, &hippo_bson_visitors, &state);
/*
		if (state->map.array || state->odm) {
			zval *obj = NULL;

			MAKE_STD_ZVAL(obj); 
			object_init_ex(obj, state->odm ? state->odm : state->map.array);
			zend_call_method_with_1_params(&obj, NULL, NULL, BSON_UNSERIALIZE_FUNC_NAME, NULL, state->zchild);
			zval_dtor(state->zchild);
			ZVAL_ZVAL(state->zchild, obj, 1, 1);
		}
*/
	} while ((b = bson_reader_read(m_reader, &eof)));

	*v = Variant(state.zchild->get());

	return true;
}
/* }}} */


} /* namespace HPHP */
