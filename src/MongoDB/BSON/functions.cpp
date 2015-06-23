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

#include "functions.h"

#include "../../../bson.h"

extern "C" {
#include "../../../libbson/src/bson/bson.h"
}

namespace HPHP {

String HHVM_FUNCTION(MongoDBBsonFromArray, const Array &data)
{
	bson_t *bson;
	String s;
	unsigned char *data_s;

	VariantToBsonConverter converter(Variant(data), HIPPO_BSON_NO_FLAGS);
	bson = bson_new();
	converter.convert(bson);

	s = String(bson->len, ReserveString);
	data_s = (unsigned char*) s.bufferSlice().ptr;
	memcpy(data_s, bson_get_data(bson), bson->len);
	s.setSize(bson->len);

	return s;
}

}
