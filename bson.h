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
		bson_t *m_output;
		int m_level;
	
	VariantToBsonConverter(const Variant& document);
	bson_t *convert(bson_t *bson);

	void convert(Variant v);
	void convert(Array a);
	void convert(Object o);

	void convertPart(const char *key);
	void convertPart(const char *key, Variant v);
	void convertPart(const char *key, bool v);
	void convertPart(const char *key, int64_t v);
	void convertPart(const char *key, double v);
	void convertPart(const char *key, String v);
	void convertPart(const char *key, Array v);
	void convertPart(const char *key, Object v);

};

}
