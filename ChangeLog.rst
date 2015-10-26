ChangeLog
=========

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

