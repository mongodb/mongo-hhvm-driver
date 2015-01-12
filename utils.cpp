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

#include "utils.h"

namespace MongoDriver
{

bool Utils::splitNamespace(HPHP::String ns, char **db, char **col)
{
	const char *input = ns.c_str();
	const char *dot = strchr(input, '.');                                      

	if (!dot) {                                                                         
		return false;        
	}                                                                            

	if (col) {                                                                        
		*col = strdup(input + (dot - input) + 1);          
	}            
	if (db) {    
		*db = strndup(input, dot - input);
	}                       

	return true;  
}

}
