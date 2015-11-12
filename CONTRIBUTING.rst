Contributing to the HHVM driver for MongoDB
===========================================

Building from VCS (GitHub)
--------------------------

Developers who would like to contribute to the driver will need to build it
from VCS like so::

	git clone https://github.com/mongodb/mongo-hhvm-driver.git hippo

	cd hippo

	git submodule update --init

	hphpize
	cmake .

	make configlib

	make -j 5
	make install

Testing
-------

To run the rests, after adjusting the paths::

	export TEST_PHP_EXECUTABLE=`which hhvm`
	hhvm -vDynamicExtensions.0=/usr/local/hhvm/3.9.1/lib/hhvm/extensions/20150212/mongodb.so run-tests.php
