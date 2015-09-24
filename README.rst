MongoDB
=======

This is the low-level HHVM driver for MongoDB. It implements the same
interface as the PHP driver for MongoDB that you will find at
https://pecl.php.net/package/mongodb. The documentation for both of them is
therefore (mostly) the same, and can be found at
http://docs.php.net/manual/en/set.mongodb.php

The driver is written to be a bare bone layer to talk to MongoDB, and
therefore misses many convenience features. Instead, these convenience methods
have been split out into a layer written in PHP, the `MongoDB Library`_.
Using this library should be your preferred way of interacting with MongoDB.

Please note that the new HHVM and PHP drivers implement a different API from
the legacy driver at http://pecl.php.net/package/mongo, and hence, you will
find it hard for other libraries like `Doctrine MongoDB's ODM`_ will not work
with the new drivers.

.. _`Doctrine MongoDB's ODM`: http://doctrine-mongodb-odm.readthedocs.org/en/latest/
.. _`MongoDB Library`: http://mongodb-labs.github.io/mongo-php-library-prototype/

Building
--------

From a package (.tgz)
~~~~~~~~~~~~~~~~~~~~~

Releases of this driver can be found on
https://s3.amazonaws.com/drivers.mongodb.org/hhvm/index.html

Please download the latest .tgz file into a directory, and run the following
commands::

	mkdir hhvm-mongodb
	cd hhvm-mongodb
	tar -xvzf ../hhvm-mongodb-1.0alpha1
	cd hhvm-mongodb-1.0alpha1
	cd libbson; ./autogen.sh; cd ..
	cd libmongoc; ./autogen.sh; cd ..
	hphpize
	cmake .
	make -j 16
	make install

From source
~~~~~~~~~~~

Compiling this extension requires the ``hphpize`` command, which is available by
installing the ``hhvm-dev`` package from the
`HHVM repositories <https://github.com/facebook/hhvm/wiki/Prebuilt-Packages-for-HHVM>`_
or `building HHVM from source <https://github.com/facebook/hhvm/wiki/Building-and-Installing-HHVM>`_.

After cloning the repository, the extension may be built like so::

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

Installing
----------

In your ``/etc/hhvm/php.ini``, add the following lines (adjusting paths if
necessary)::

	hhvm.dynamic_extension_path=/usr/local/hhvm/3.9.1/lib/hhvm/extensions/20150212
	hhvm.dynamic_extensions[mongodb]=mongodb.so

Running Tests
-------------

To run the rests::

	export TEST_PHP_EXECUTABLE=`which hhvm`
	hhvm -vDynamicExtensions.0=/usr/local/hhvm/3.9.1/lib/hhvm/extensions/20150212/mongodb.so run-tests.php
