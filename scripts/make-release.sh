#!/bin/sh

version=$1

rm -rf /tmp/hhvm-build
mkdir -p /tmp/hhvm-build
cd /tmp/hhvm-build
git clone git@github.com:mongodb/mongo-hhvm-driver.git hhvm-mongodb-${version}
cd hhvm-mongodb-${version}
git checkout "$version"
git submodule init
git submodule update

#removing nonsense
rm -rf /tmp/hhvm-build/hhvm-mongodb-${version}/.git

cd -
tar cvzf /tmp/hhvm-mongodb-${version}.tgz hhvm-mongodb-${version}
