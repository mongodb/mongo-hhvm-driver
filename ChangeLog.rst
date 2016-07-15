ChangeLog
=========

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

