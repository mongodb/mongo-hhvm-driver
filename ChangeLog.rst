CHANGELOG
=========

1.2.4
-----

** Bug
    * [HHVM-287] - Using a projection with an empty field name causes a crash when destroying cursor
    * [HHVM-293] - API breaks in HHVM 3.18 and 3.19


1.2.3
-----

** Bug
    * [HHVM-284] - readConcern option should not be included in getMore commands


1.2.2
-----

** Bug
    * [HHVM-281] - Fix compile issue with HHVM 3.17


1.2.1
-----

** Bug
    * [HHVM-280] - Assertion failure in stream_not_found() after failed getmore operation
    * [HHVM-281] - Fix compile issue with HHVM 3.17
    * [HHVM-282] - Alphabetize Regex flags when instantiating from BSON


1.2.0
-----

** Bug
    * [HHVM-215] - Free server descriptions with mongoc_server_descriptions_destroy_all()
    * [HHVM-224] - Unacknowledged WriteResults have null counts
    * [HHVM-225] - Do not use mongoc_cursor_t private API in Cursor debug handler
    * [HHVM-241] - Fix compile issue with HHVM 3.15
    * [HHVM-248] - Validate that read preference tags is an array of documents
    * [HHVM-251] - UTCDateTime should support microseconds
    * [HHVM-252] - BulkWrite::update() silently ignores invalid arguments
    * [HHVM-255] - Query execution should not assign read concern to client
    * [HHVM-257] - Empty ReadConcern and WriteConcern should serialize to BSON documents
    * [HHVM-266] - Javascript::jsonSerialize() should return a document with $code and $scope fields
    * [HHVM-273] - fromJSON() should not evaluate bson_error_t.message as boolean
    * [HHVM-274] - Javascript object serialization in libmongoc changed
    * [HHVM-275] - ObjectIDs don't compare properly
    * [HHVM-276] - Back out APM from 1.2
    * [HHVM-277] - Mark \MongoDB\Driver\Manager as final

** Epic
    * [HHVM-165] - Remove dependency on mongoc private symbols

** Improvement
    * [HHVM-123] - Improve connection handling
    * [HHVM-158] - BSON objects should implement JsonSerializable
    * [HHVM-173] - Rephrase unsupported/corrupt BSON messages
    * [HHVM-195] - WriteResult debug handler should return objects
    * [HHVM-221] - Report bypassDocumentValidation in BulkWrite debug output
    * [HHVM-222] - Do not allow BulkWrite objects to be executed multiple times
    * [HHVM-229] - Javascript constructor should throw if code contains null bytes
    * [HHVM-230] - Regex constructor should throw if pattern or flags contain null bytes
    * [HHVM-234] - Rename "javascript" to "code" in Javascript BSON class
    * [HHVM-235] - Regex constructor should default flags arg to empty string
    * [HHVM-236] - Improve error messages for invalid Decimal128 and ObjectID strings
    * [HHVM-237] - Default Manager URI to "mongodb://127.0.0.1/"
    * [HHVM-258] - Ensure read preference tags sets serialize as documents
    * [HHVM-267] - UTCDateTime::jsonSerialize() should return a $numberLong field

** New Feature
    * [HHVM-130] - BSON classes should support PHP serialization and var_export()
    * [HHVM-134] - BulkWrite::insert() should always return the document's ID
    * [HHVM-151] - Add code and scope getters to Javascript BSON object
    * [HHVM-162] - UTCDateTime constructor should default to current time and accept DateTimeInterface
    * [HHVM-176] - ReadPreference, ReadConcern, and WriteConcern should serialize to BSON
    * [HHVM-186] - Implement Decimal 128 type spec
    * [HHVM-189] - Implement SDAM Monitoring spec
    * [HHVM-190] - Implement APM Specification
    * [HHVM-216] - Implement ObjectID::getTimestamp() method
    * [HHVM-226] - Support sending writeConcern for commands that write
    * [HHVM-228] - Support new readConcern level "linearizable"
    * [HHVM-231] - Support providing collation per operation
    * [HHVM-232] - Implement __toString() method for Javascript and Binary
    * [HHVM-233] - Implement getCode() and getScope() for Javascript
    * [HHVM-239] - Allow users to set a limit on acceptable staleness
    * [HHVM-240] - Implement The MongoDB Handshake Protocol
    * [HHVM-250] - Support appname in URI array options

** Task
    * [HHVM-184] - Make sure the driver compiles with HHVM 3.13.
    * [HHVM-196] - Manager::getServers() and ::__debugInfo() should use mongoc_client_get_server_descriptions()
    * [HHVM-197] - Create command cursors with mongoc_cursor_new_from_command_document()
    * [HHVM-198] - Use mongoc_read_prefs_t getters instead of accessing struct fields directly
    * [HHVM-199] - WriteConcern::getJournal() should use mongoc_write_concern_journal_is_set()
    * [HHVM-200] - Use mongoc_server_description_t public API in Server methods
    * [HHVM-201] - Manager::selectServer() should use mongoc_client_select_server()
    * [HHVM-202] - Ensure client URI handling uses libmongoc public API
    * [HHVM-203] - Client construction should use mongoc_write_concern_is_valid()
    * [HHVM-204] - WriteResult::isAcknowledged() should use mongoc_write_concern_is_acknowledged()
    * [HHVM-205] - Upgrade libbson and libmongoc to 1.4.0
    * [HHVM-206] - Update bson_visitor_t for unsupported type function pointer
    * [HHVM-208] - Update bson_visitor_t for decimal 128 type function pointer
    * [HHVM-210] - BulkWrite::__debugInfo should use mongoc_bulk_operation_get_write_concern()
    * [HHVM-211] - Remove private libmongoc and libbson header includes
    * [HHVM-212] - Cursor::getServer() should not access mongoc_cursor_t.client
    * [HHVM-219] - BulkWrite::count() should return number of operations instead of estimated round-trips
    * [HHVM-220] - Do not use mongoc_bulk_operation_t private API in BulkWrite debug handler
    * [HHVM-223] - Query and command execution should use mongoc_cursor_set_hint()
    * [HHVM-245] - Implement alternative names for SSL options
    * [HHVM-246] - mongoc-metadata*[c,h] renamed to mongoc-handshake*[c,h]
    * [HHVM-247] - Javascript serialization, export, and dump should always include scope field
    * [HHVM-249] - Use mongoc_collection_find_with_opts() for Query execution
    * [HHVM-253] - Upgrade libbson and libmongoc to 1.5.0
    * [HHVM-254] - Use flexible opts for BulkWrite update and delete
    * [HHVM-256] - Create notice for all third party libraries
    * [HHVM-259] - Query "partial" option is now "allowPartialResults"
    * [HHVM-262] - SDAM spec update : Update the topology from each handshake
    * [HHVM-264] - BSON Regex flags must be alphabetically ordered
    * [HHVM-265] - Update Max Staleness implementation


1.2.0alpha1
-----------

** Bug
    * [HHVM-215] - Free server descriptions with mongoc_server_descriptions_destroy_all()
    * [HHVM-225] - Do not use mongoc_cursor_t private API in Cursor debug handler
    * [HHVM-248] - Validate that read preference tags is an array of documents
    * [HHVM-251] - UTCDateTime should support microseconds
    * [HHVM-252] - BulkWrite::update() silently ignores invalid arguments

** Epic
    * [HHVM-165] - Remove dependency on mongoc private symbols

** Improvement
    * [HHVM-221] - Report bypassDocumentValidation in BulkWrite debug output
    * [HHVM-222] - Do not allow BulkWrite objects to be executed multiple times
    * [HHVM-229] - Javascript constructor should throw if code contains null bytes
    * [HHVM-234] - Rename "javascript" to "code" in Javascript BSON class
    * [HHVM-235] - Regex constructor should default flags arg to empty string
    * [HHVM-236] - Improve error messages for invalid Decimal128 and ObjectID strings
    * [HHVM-237] - Default Manager URI to "mongodb://127.0.0.1/"

** New Feature
    * [HHVM-130] - BSON classes should support PHP serialization and var_export()
    * [HHVM-151] - Add code and scope getters to Javascript BSON object
    * [HHVM-162] - UTCDateTime constructor should default to current time and accept DateTimeInterface
    * [HHVM-176] - ReadPreference, ReadConcern, and WriteConcern should serialize to BSON
    * [HHVM-186] - Implement Decimal 128 type spec
    * [HHVM-189] - Implement SDAM Monitoring spec
    * [HHVM-226] - Support sending writeConcern for commands that write
    * [HHVM-228] - Support new readConcern level "linearizable"
    * [HHVM-231] - Support providing collation per operation
    * [HHVM-232] - Implement __toString() method for Javascript and Binary
    * [HHVM-233] - Implement getCode() and getScope() for Javascript
    * [HHVM-239] - Allow users to set a limit on acceptable staleness
    * [HHVM-240] - Implement The MongoDB Handshake Protocol
    * [HHVM-250] - Support appname in URI array options

** Task
    * [HHVM-184] - Make sure the driver compiles with HHVM 3.13.
    * [HHVM-196] - Manager::getServers() and ::__debugInfo() should use mongoc_client_get_server_descriptions()
    * [HHVM-197] - Create command cursors with mongoc_cursor_new_from_command_document()
    * [HHVM-198] - Use mongoc_read_prefs_t getters instead of accessing struct fields directly
    * [HHVM-199] - WriteConcern::getJournal() should use mongoc_write_concern_journal_is_set()
    * [HHVM-200] - Use mongoc_server_description_t public API in Server methods
    * [HHVM-201] - Manager::selectServer() should use mongoc_client_select_server()
    * [HHVM-202] - Ensure client URI handling uses libmongoc public API
    * [HHVM-203] - Client construction should use mongoc_write_concern_is_valid()
    * [HHVM-204] - WriteResult::isAcknowledged() should use mongoc_write_concern_is_acknowledged()
    * [HHVM-205] - Upgrade libbson and libmongoc to 1.4.0
    * [HHVM-206] - Update bson_visitor_t for unsupported type function pointer
    * [HHVM-208] - Update bson_visitor_t for decimal 128 type function pointer
    * [HHVM-210] - BulkWrite::__debugInfo should use mongoc_bulk_operation_get_write_concern()
    * [HHVM-211] - Remove private libmongoc and libbson header includes
    * [HHVM-212] - Cursor::getServer() should not access mongoc_cursor_t.client
    * [HHVM-214] - Implement interfaces for userland BSON type classes
    * [HHVM-219] - BulkWrite::count() should return number of operations instead of estimated round-trips
    * [HHVM-220] - Do not use mongoc_bulk_operation_t private API in BulkWrite debug handler
    * [HHVM-223] - Query and command execution should use mongoc_cursor_set_hint()
    * [HHVM-245] - Implement alternative names for SSL options
    * [HHVM-246] - mongoc-metadata*[c,h] renamed to mongoc-handshake*[c,h]
    * [HHVM-247] - Javascript serialization, export, and dump should always include scope field
    * [HHVM-249] - Use mongoc_collection_find_with_opts() for Query execution
    * [HHVM-254] - Use flexible opts for BulkWrite update and delete


1.1.3
-----

** Bug
    * [HHVM-238] - Driver does not compile with HHVM 3.14


1.1.2
-----

** Bug
    * [HHVM-209] - Driver fails to build due to "Logger" with HHVM 3.9-3.11

** Task
    * [HHVM-161] - WriteResult should encapsulate BSON instead of mongoc_write_result_t
    * [HHVM-191] - Use bson_error_t message for BulkWriteException message
    * [HHVM-193] - executeBulkWrite() should throw InvalidArgumentException for empty BulkWrite


1.1.1
-----

** Bug
    * [HHVM-183] - toJSON() should throw on invalid BSON
    * [HHVM-185] - Throw exception for failed root or nested BSON iteration
    * [HHVM-188] - ObjectID only supports lower case hexadecimal letters

** Task
    * [HHVM-194] - Upgrade bundled libbson and libmongoc to 1.3.4


1.1.0
-----

** Bug
    * [HHVM-137] - toJSON and fromJSON should throw exceptions on errors
    * [HHVM-149] - Make sure we bundle all source code in release tarballs
    * [HHVM-157] - The Ghost bug
    * [HHVM-163] - HHVM 3.11 adds additional data types that we need to support
    * [HHVM-164] - Bug with BulkWrite->update() and choosing between replace() or update()
    * [HHVM-167] - Parse readconcernlevel in URI options array
    * [HHVM-168] - Driver does not pick on up default writeConcern
    * [HHVM-170] - Make it work with HHVM 3.12
    * [HHVM-174] - Autoloading is not triggered for user defined classes
    * [HHVM-175] - Only set readConcern when it's 'local' or 'majority'

** Epic
    * [HHVM-111] - MongoDB 3.2 Compatibility

** Improvement
    * [HHVM-148] - Use more descriptive messages in WriteExceptions
    * [HHVM-153] - Query debug handler should defer to ReadConcern handler for readConcern option
    * [HHVM-171] - Manager::selectServer() should select exception class based on bson_error_t
    * [HHVM-178] - Throw exception if Binary subtype is out of range
    * [HHVM-179] - Throw exception if WriteConcern wtimeout is out of range
    * [HHVM-180] - Throw exception if Timestamp arguments are out of range

** New Feature
    * [HHVM-132] - Implement WriteConcernError::getInfo()

** Question
    * [HHVM-152] - Research whether array-casting on Query projection is necessary

** Task
    * [HHVM-143] - Update libmongoc/libbson to 1.2.1
    * [HHVM-156] - Upgrade bundled libbson and libmongoc to 1.3.2
    * [HHVM-159] - Remove request_id from the Manager's __debugInfo()
    * [HHVM-160] - Upgrade libbson and libmongoc to 1.3.3
    * [HHVM-166] - Remove undocumented Manager "hosts" URI option
    * [HHVM-169] - Always encode ODS field when serializing Persistable documents

1.1.0RC1
--------

** Bug
    * [HHVM-154] - Link in OpenSSL/SASL and don't rely on other bits to do that

** Improvement
    * [HHVM-116] - Support bypassDocumentValidation option for insert and update commands
    * [HHVM-150] - Default to IPv4 localhost address for Manager constructor
    * [HHVM-155] - Upgrade bundled libbson and libmongoc to 1.3.1

** New Feature
    * [HHVM-110] - Support new commands for find, getMore, and killCursors
    * [HHVM-120] - Ensure spec compliance for unacknowledged write concerns.
    * [HHVM-145] - Add support for the readConcern option

** Task
    * [HHVM-97] - Use constant-time hash comparison functions
    * [HHVM-140] - Update cursor iteration for libmongoc refactoring
    * [HHVM-144] - Update libmongoc/libbson to 1.3.0

1.0.0RC1
--------

** Improvement
    * [HHVM-117] - BulkWrite and executeInsert() should support an options array

** Task
    * [HHVM-124] - Upgrade bundled libbson and libmongoc to 1.2.0
    * [HHVM-125] - Remove WriteResult::getInfo() method
    * [HHVM-126] - Classes should be final unless inheritance is necessary
    * [HHVM-127] - Remove DuplicateKeyException
    * [HHVM-128] - Create common parent for write concern and write errors
    * [HHVM-129] - Remove single write methods, WriteErrorException, and WriteConcernException
    * [HHVM-131] - Add tutorial on how to get started with the driver and PHPLIB

1.0.0beta1
----------

** Task
    * [HHVM-58] - Implement logging through HHVM's logging mechanism
    * [HHVM-113] - Handle new writeConcernErrors array in mongoc_write_result_t
    * [HHVM-114] - WriteResult debug handler should display null for a missing writeConcernError
    * [HHVM-118] - Upgrade bundled libmongoc and libbson to 1.2.0-rc0

** Bug
    * [HHVM-121] - Default DSN is not parsable

