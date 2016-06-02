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

#include "hphp/runtime/ext/datetime/ext_datetime.h"

#include "UTCDateTime.h"

namespace HPHP {

const StaticString s_MongoBsonUTCDateTime_className("MongoDB\\BSON\\UTCDateTime");

const StaticString s_MongoBsonUTCDateTime_milliseconds("milliseconds");

const StaticString s_DateTime("DateTime");

Object HHVM_METHOD(MongoDBBsonUTCDateTime, toDateTime)
{
	int64_t milliseconds = this_->o_get(s_MongoBsonUTCDateTime_milliseconds, false, s_MongoBsonUTCDateTime_className).toInt64();

	/* Prepare result */
	HPHP::Object obj{DateTimeData::getClass()};

	DateTimeData* data = Native::data<DateTimeData>(obj.get());
#if HIPPO_HHVM_VERSION >= 30900
	data->m_dt = req::make<DateTime>(milliseconds / 1000, true);
#else
	data->m_dt = makeSmartPtr<DateTime>(milliseconds / 1000, true);
#endif

	/* It would be nice to do this, but HHVM doesn't export this yet
	 *
	 * data->m_dt->fraction( ( (double)(milliseconds % 1000) ) / 1000);
	 */

	data->m_dt->setTimezone(req::make<TimeZone>(String("UTC")));

	return obj;
}

}
