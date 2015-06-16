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

#include "hphp/runtime/base/base-includes.h"

extern "C" {
#include "libbson/src/bson/bson.h"
}

namespace HPHP {

#define HIPPO_BSON_NO_FLAGS   0x00
#define HIPPO_BSON_ADD_ID     0x01
#define HIPPO_BSON_RETURN_ID  0x02

#define HIPPO_TYPEMAP_STDCLASS 0x04
#define HIPPO_TYPEMAP_ARRAY    0x05

class VariantToBsonConverter
{
	public:
		Variant m_document;
		bson_t *m_out;

/* {{{ public methods */
		VariantToBsonConverter(const Variant& document, int flags);
		void convert(bson_t *bson);

		void convert(bson_t *bson, Variant v);
		void convert(bson_t *bson, Array a);
		void convert(bson_t *bson, Object o);

		void convertPart(bson_t *bson, const char *key);
		void convertPart(bson_t *bson, const char *key, Variant v);
		void convertPart(bson_t *bson, const char *key, bool v);
		void convertPart(bson_t *bson, const char *key, int64_t v);
		void convertPart(bson_t *bson, const char *key, double v);
		void convertPart(bson_t *bson, const char *key, String v);
		void convertPart(bson_t *bson, const char *key, Array v, bool wrap, bool from_object);
		void convertPart(bson_t *bson, const char *key, Object v);
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
		void _convertJavascript(bson_t *bson, const char *key, Object v);
		void _convertMaxKey(bson_t *bson, const char *key, Object v);
		void _convertMinKey(bson_t *bson, const char *key, Object v);
		void _convertObjectId(bson_t *bson, const char *key, Object v);
		void _convertRegex(bson_t *bson, const char *key, Object v);
		void _convertTimestamp(bson_t *bson, const char *key, Object v);
		void _convertUtcDatetime(bson_t *bson, const char *key, Object v);
/* }}} */
};

typedef struct {
	int array_type;
	int document_type;
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

		hippo_bson_conversion_options_t m_options;
};

}
#endif
