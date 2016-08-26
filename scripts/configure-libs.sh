#!/bin/sh

cd libbson
if [ -f Makefile ]; then
	make distclean
fi
./autogen.sh --enable-decimal-bid=no --enable-experimental-features
cd -

cd libmongoc
if [ -f Makefile ]; then
	make distclean
fi
./autogen.sh --enable-experimental-features
cd -
