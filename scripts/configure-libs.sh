#!/bin/sh

cd libbson
if [ -f Makefile ]; then
	make distclean
fi
./autogen.sh --enable-decimal-bid=no
cd -

cd libmongoc
if [ -f Makefile ]; then
	make distclean
fi
./autogen.sh
cd -
