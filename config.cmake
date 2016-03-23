HHVM_EXTENSION(mongodb
 mongodb.cpp bson.cpp utils.cpp
 src/MongoDB/BSON/functions.cpp
 src/MongoDB/BSON/Binary.cpp
 src/MongoDB/BSON/Javascript.cpp
 src/MongoDB/BSON/ObjectID.cpp
 src/MongoDB/BSON/Regex.cpp
 src/MongoDB/BSON/Timestamp.cpp
 src/MongoDB/BSON/UTCDateTime.cpp
 src/MongoDB/Driver/BulkWrite.cpp
 src/MongoDB/Driver/Command.cpp
 src/MongoDB/Driver/Cursor.cpp
 src/MongoDB/Driver/CursorId.cpp
 src/MongoDB/Driver/Manager.cpp
 src/MongoDB/Driver/Query.cpp
 src/MongoDB/Driver/ReadConcern.cpp
 src/MongoDB/Driver/ReadPreference.cpp
 src/MongoDB/Driver/Server.cpp
 src/MongoDB/Driver/WriteConcern.cpp
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
 libbson/src/bson/bson-value.c libbson/src/bson/bson-version-functions.c
 libbson/src/bson/bson-writer.c libbson/src/bson/bson.c
 libmongoc/src/mongoc/mongoc-apm.c
 libmongoc/src/mongoc/mongoc-async.c
 libmongoc/src/mongoc/mongoc-async-cmd.c
 libmongoc/src/mongoc/mongoc-array.c libmongoc/src/mongoc/mongoc-b64.c
 libmongoc/src/mongoc/mongoc-buffer.c
 libmongoc/src/mongoc/mongoc-bulk-operation.c
 libmongoc/src/mongoc/mongoc-client-pool.c libmongoc/src/mongoc/mongoc-client.c
 libmongoc/src/mongoc/mongoc-cluster.c libmongoc/src/mongoc/mongoc-collection.c
 libmongoc/src/mongoc/mongoc-counters.c
 libmongoc/src/mongoc/mongoc-crypto.c
 libmongoc/src/mongoc/mongoc-crypto-common-crypto.c
 libmongoc/src/mongoc/mongoc-crypto-openssl.c
 libmongoc/src/mongoc/mongoc-cursor-array.c
 libmongoc/src/mongoc/mongoc-cursor-cursorid.c
 libmongoc/src/mongoc/mongoc-cursor-transform.c
 libmongoc/src/mongoc/mongoc-cursor.c libmongoc/src/mongoc/mongoc-database.c
 libmongoc/src/mongoc/mongoc-find-and-modify.c
 libmongoc/src/mongoc/mongoc-gridfs-file-list.c
 libmongoc/src/mongoc/mongoc-gridfs-file-page.c
 libmongoc/src/mongoc/mongoc-gridfs-file.c libmongoc/src/mongoc/mongoc-gridfs.c
 libmongoc/src/mongoc/mongoc-host-list.c
 libmongoc/src/mongoc/mongoc-index.c libmongoc/src/mongoc/mongoc-init.c
 libmongoc/src/mongoc/mongoc-list.c libmongoc/src/mongoc/mongoc-log.c
 libmongoc/src/mongoc/mongoc-matcher-op.c libmongoc/src/mongoc/mongoc-matcher.c
 libmongoc/src/mongoc/mongoc-memcmp.c
 libmongoc/src/mongoc/mongoc-opcode.c
 libmongoc/src/mongoc/mongoc-openssl.c
 libmongoc/src/mongoc/mongoc-queue.c
 libmongoc/src/mongoc/mongoc-rand-common-crypto.c
 libmongoc/src/mongoc/mongoc-rand-openssl.c
 libmongoc/src/mongoc/mongoc-read-concern.c
 libmongoc/src/mongoc/mongoc-read-prefs.c libmongoc/src/mongoc/mongoc-rpc.c
 libmongoc/src/mongoc/mongoc-sasl.c libmongoc/src/mongoc/mongoc-scram.c
 libmongoc/src/mongoc/mongoc-secure-transport.c
 libmongoc/src/mongoc/mongoc-server-description.c
 libmongoc/src/mongoc/mongoc-server-stream.c
 libmongoc/src/mongoc/mongoc-set.c
 libmongoc/src/mongoc/mongoc-socket.c libmongoc/src/mongoc/mongoc-ssl.c
 libmongoc/src/mongoc/mongoc-stream-buffered.c
 libmongoc/src/mongoc/mongoc-stream-file.c
 libmongoc/src/mongoc/mongoc-stream-gridfs.c
 libmongoc/src/mongoc/mongoc-stream-socket.c
 libmongoc/src/mongoc/mongoc-stream-tls.c
 libmongoc/src/mongoc/mongoc-stream-tls-openssl.c
 libmongoc/src/mongoc/mongoc-stream-tls-openssl-bio.c
 libmongoc/src/mongoc/mongoc-stream-tls-secure-transport.c
 libmongoc/src/mongoc/mongoc-stream.c
 libmongoc/src/mongoc/mongoc-topology.c
 libmongoc/src/mongoc/mongoc-topology-description.c
 libmongoc/src/mongoc/mongoc-topology-scanner.c
 libmongoc/src/mongoc/mongoc-uri.c libmongoc/src/mongoc/mongoc-util.c
 libmongoc/src/mongoc/mongoc-version-functions.c
 libmongoc/src/mongoc/mongoc-write-command.c
 libmongoc/src/mongoc/mongoc-write-concern.c
)
FIND_PACKAGE(OpenSSL)
HHVM_LINK_LIBRARIES(mongodb ${OPENSSL_LIBRARIES})
find_library(SASL_LIBRARIES NAMES sasl2)
HHVM_LINK_LIBRARIES(mongodb ${SASL_LIBRARIES})

HHVM_DEFINE(mongodb "-DBSON_COMPILATION=1")
HHVM_DEFINE(mongodb "-DMONGOC_COMPILATION=1")
HHVM_DEFINE(mongodb "-DMONGOC_ENABLE_CRYPTO")
HHVM_DEFINE(mongodb "-DMONGOC_ENABLE_SSL")
HHVM_DEFINE(mongodb "-DMONGOC_ENABLE_SASL")
HHVM_DEFINE(mongodb "-DMONGOC_TRACE=1")
HHVM_ADD_INCLUDES(mongodb libbson/src)
HHVM_ADD_INCLUDES(mongodb libbson/src/bson)
HHVM_SYSTEMLIB(mongodb ext_mongodb.php)

add_custom_target(configlib
  COMMAND "scripts/configure-libs.sh")
