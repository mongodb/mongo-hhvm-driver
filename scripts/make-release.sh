#!/bin/sh

version=$1

rm -rf /tmp/hhvm-build
mkdir -p /tmp/hhvm-build
cd /tmp/hhvm-build
git clone git@github.com:mongodb-labs/mongo-hhvm-driver-prototype.git hhvm-mongo-${version}
cd hhvm-mongo-${version}
git checkout "$version"
git submodule init
git submodule update

#removing nonsense
rm -rf /tmp/hhvm-build/hhvm-mongo-${version}/.git
rm -rf /tmp/hhvm-build/hhvm-mongo-${version}/libbson/doc
rm -rf /tmp/hhvm-build/hhvm-mongo-${version}/libbson/tests
rm -rf /tmp/hhvm-build/hhvm-mongo-${version}/libmongoc/doc
rm -rf /tmp/hhvm-build/hhvm-mongo-${version}/libmongoc/examples
rm -rf /tmp/hhvm-build/hhvm-mongo-${version}/libmongoc/orchestration_configs



cd -
tar cvzf /tmp/hhvm-mongodb-${version}.tgz hhvm-mongo-${version}
