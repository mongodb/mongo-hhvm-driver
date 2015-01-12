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
#include "hphp/runtime/base/base-includes.h"

extern "C" {
#include "libbson/src/bson/bson.h"
}

namespace HPHP {

class VariantToBsonConverter
{
	public:
		Variant m_document;
		int m_level;
/* {{{ public methods */
		VariantToBsonConverter(const Variant& document);
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
		void convertPart(bson_t *bson, const char *key, Array v);
		void convertPart(bson_t *bson, const char *key, Object v);
/* }}} */
	private:
/* private methods {{{ */
		int _isPackedArray(const Array &a);
		char *_getUnmangledPropertyName(String key);
/* }}} */
};

}
