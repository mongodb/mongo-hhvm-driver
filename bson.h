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
#ifndef __MONGODB_BSON_H__
#define __MONGODB_BSON_H__

#include "hphp/runtime/ext/extension.h"

extern "C" {
#include "libbson/src/bson/bson.h"
}

namespace HPHP {

/* This is a bitfield */
#define HIPPO_BSON_NO_FLAGS    0x00
#define HIPPO_BSON_ADD_ID      0x01
#define HIPPO_BSON_RETURN_ID   0x02

/* This is not a bitfield */
#define HIPPO_TYPEMAP_DEFAULT    0x03
#define HIPPO_TYPEMAP_STDCLASS   0x04
#define HIPPO_TYPEMAP_ARRAY      0x05
#define HIPPO_TYPEMAP_NAMEDCLASS 0x06

#define HIPPO_BSONTYPE_ARRAY     0x10
#define HIPPO_BSONTYPE_DOCUMENT  0x11
#define HIPPO_BSONTYPE_ROOT      0x12

#define HIPPO_TYPEMAP_INITIALIZER { HIPPO_TYPEMAP_DEFAULT, HIPPO_TYPEMAP_DEFAULT, HIPPO_TYPEMAP_DEFAULT, HIPPO_BSONTYPE_ROOT }
#define HIPPO_TYPEMAP_DEBUG_INITIALIZER { HIPPO_TYPEMAP_ARRAY, HIPPO_TYPEMAP_ARRAY, HIPPO_TYPEMAP_ARRAY, HIPPO_BSONTYPE_ROOT }

class VariantToBsonConverter
{
	public:
		Variant m_document;
		Variant m_out;

/* {{{ public methods */
		VariantToBsonConverter(const Variant& document, int flags);
		void convert(bson_t *bson);
/* }}} */
	private:
/* private properties {{{ */
		int m_level;
		int m_flags;
/* }}} */
/* private methods {{{ */
		int _isPackedArray(const Array &a);
		char *_getUnmangledPropertyName(String key);
		void _convertBinary(bson_t *bson, const char *key, Object v);
		void _convertDecimal128(bson_t *bson, const char *key, Object v);
		void _convertJavascript(bson_t *bson, const char *key, Object v);
		void _convertMaxKey(bson_t *bson, const char *key, Object v);
		void _convertMinKey(bson_t *bson, const char *key, Object v);
		void _convertObjectID(bson_t *bson, const char *key, Object v);
		void _convertRegex(bson_t *bson, const char *key, Object v);
		void _convertTimestamp(bson_t *bson, const char *key, Object v);
		void _convertUTCDateTime(bson_t *bson, const char *key, Object v);

		void _convertCursorId(bson_t *bson, const char *key, Object v);

		void _convertSerializable(bson_t *bson, const char *key, Object v);

		void convertDocument(bson_t *bson, const char *key, Variant v);
		void convertElement(bson_t *bson, const char *key, Variant v);
		bool convertSpecialObject(bson_t *bson, const char *key, Object v);

		void convertNull(bson_t *bson, const char *key);
		void convertBoolean(bson_t *bson, const char *key, bool v);
		void convertInt64(bson_t *bson, const char *key, int64_t v);
		void convertDouble(bson_t *bson, const char *key, double v);
		void convertString(bson_t *bson, const char *key, String v);
/* }}} */
};

typedef struct {
	int array_type;
	int root_type;
	int document_type;
	int current_compound_type;
	String array_class_name;
	String root_class_name;
	String document_class_name;

	struct {
		String binary_class_name;
		String decimal128_class_name;
		String javascript_class_name;
		String maxkey_class_name;
		String minkey_class_name;
		String objectid_class_name;
		String regex_class_name;
		String timestamp_class_name;
		String utcdatetime_class_name;
	} types;
} hippo_bson_conversion_options_t;

typedef struct {
	Array zchild;
	hippo_bson_conversion_options_t options;
} hippo_bson_state;

class BsonToVariantConverter
{
	public:
		BsonToVariantConverter(const unsigned char *data, int data_len, hippo_bson_conversion_options_t options);
		bool convert(Variant *v);
	private:
		bson_reader_t *m_reader;

		hippo_bson_state m_state;
		hippo_bson_conversion_options_t m_options;
};

/* {{{ TypeMap helper functions */
void parseTypeMap(hippo_bson_conversion_options_t *options, const Array &typemap);
/* }}} */

}
#endif
