MongoDB
=======

This is a proof-of-concept HHVM extension mimicking the proof-of-concept PHP
extension "phongo".

Building
--------

To build::

	git submodule init
	git submodule update

	cd libbson
	./autogen.sh
	cd ..

	cd libmongoc
	./autogen.sh
	cd ..

	hphpize
	cmake .

	make -j 5
	make install

Running
-------

To run the rests::

	export TEST_PHP_EXECUTABLE=`which hhvm`
	hhvm -vDynamicExtensions=mongodb.so run-tests.php
