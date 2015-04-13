HHVM_EXTENSION(mongodb
 mongodb.cpp bson.cpp utils.cpp
 src/MongoDB/BSON/Binary.cpp
 src/MongoDB/BSON/Javascript.cpp
 src/MongoDB/BSON/ObjectId.cpp
 src/MongoDB/BSON/Regex.cpp
 src/MongoDB/BSON/Timestamp.cpp
 src/MongoDB/BSON/UtcDatetime.cpp
 src/MongoDB/Driver/BulkWrite.cpp
 src/MongoDB/Driver/Command.cpp
 src/MongoDB/Driver/Cursor.cpp
 src/MongoDB/Driver/CursorId.cpp
 src/MongoDB/Driver/Manager.cpp
 src/MongoDB/Driver/Query.cpp
 src/MongoDB/Driver/ReadPreference.cpp
 src/MongoDB/Driver/Server.cpp
 src/MongoDB/Driver/WriteResult.cpp
 libbson/src/yajl/yajl.c libbson/src/yajl/yajl_alloc.c
 libbson/src/yajl/yajl_buf.c libbson/src/yajl/yajl_encode.c
 libbson/src/yajl/yajl_gen.c libbson/src/yajl/yajl_lex.c
 libbson/src/yajl/yajl_parser.c libbson/src/yajl/yajl_tree.c
 libbson/src/yajl/yajl_version.c
 libbson/src/bson/bcon.c libbson/src/bson/bson-atomic.c
 libbson/src/bson/bson-clock.c libbson/src/bson/bson-context.c
 libbson/src/bson/bson-error.c libbson/src/bson/bson-iso8601.c
 libbson/src/bson/bson-iter.c libbson/src/bson/bson-json.c
 libbson/src/bson/bson-keys.c libbson/src/bson/bson-md5.c
 libbson/src/bson/bson-memory.c libbson/src/bson/bson-oid.c
 libbson/src/bson/bson-reader.c libbson/src/bson/bson-string.c
 libbson/src/bson/bson-timegm.c libbson/src/bson/bson-utf8.c
 libbson/src/bson/bson-value.c libbson/src/bson/bson-version.c
 libbson/src/bson/bson-writer.c libbson/src/bson/bson.c
 libmongoc/src/mongoc/mongoc-array.c libmongoc/src/mongoc/mongoc-buffer.c
 libmongoc/src/mongoc/mongoc-bulk-operation.c
 libmongoc/src/mongoc/mongoc-client-pool.c libmongoc/src/mongoc/mongoc-client.c
 libmongoc/src/mongoc/mongoc-cluster.c libmongoc/src/mongoc/mongoc-collection.c
 libmongoc/src/mongoc/mongoc-counters.c
 libmongoc/src/mongoc/mongoc-cursor-array.c
 libmongoc/src/mongoc/mongoc-cursor-cursorid.c
 libmongoc/src/mongoc/mongoc-cursor.c libmongoc/src/mongoc/mongoc-database.c
 libmongoc/src/mongoc/mongoc-gridfs-file-list.c
 libmongoc/src/mongoc/mongoc-gridfs-file-page.c
 libmongoc/src/mongoc/mongoc-gridfs-file.c libmongoc/src/mongoc/mongoc-gridfs.c
 libmongoc/src/mongoc/mongoc-index.c libmongoc/src/mongoc/mongoc-init.c
 libmongoc/src/mongoc/mongoc-list.c libmongoc/src/mongoc/mongoc-log.c
 libmongoc/src/mongoc/mongoc-matcher-op.c libmongoc/src/mongoc/mongoc-matcher.c
 libmongoc/src/mongoc/mongoc-queue.c libmongoc/src/mongoc/mongoc-rand.c
 libmongoc/src/mongoc/mongoc-read-prefs.c libmongoc/src/mongoc/mongoc-rpc.c
 libmongoc/src/mongoc/mongoc-sasl.c libmongoc/src/mongoc/mongoc-scram.c
 libmongoc/src/mongoc/mongoc-socket.c libmongoc/src/mongoc/mongoc-ssl.c
 libmongoc/src/mongoc/mongoc-stream-buffered.c
 libmongoc/src/mongoc/mongoc-stream-file.c
 libmongoc/src/mongoc/mongoc-stream-gridfs.c
 libmongoc/src/mongoc/mongoc-stream-socket.c
 libmongoc/src/mongoc/mongoc-stream-tls.c libmongoc/src/mongoc/mongoc-stream.c
 libmongoc/src/mongoc/mongoc-uri.c libmongoc/src/mongoc/mongoc-util.c
 libmongoc/src/mongoc/mongoc-write-command.c
 libmongoc/src/mongoc/mongoc-write-concern.c
)
HHVM_DEFINE(mongodb "-DBSON_COMPILATION=1")
HHVM_DEFINE(mongodb "-DMONGOC_COMPILATION=1")
HHVM_ADD_INCLUDES(mongodb libbson/src)
HHVM_ADD_INCLUDES(mongodb libbson/src/bson)
HHVM_SYSTEMLIB(mongodb ext_mongodb.php)
