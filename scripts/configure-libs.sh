#!/bin/sh

cd libbson
if [ -f Makefile ]; then
	make distclean
fi
./autogen.sh
cd -

cd libmongoc
if [ -f Makefile ]; then
	make distclean
fi
./autogen.sh
cd -
