MongoDB
=======

This is the low-level HHVM driver for MongoDB. The API is the same as the PHP
driver for MongoDB. The documentation for both of them is the same, and can be
found at http://www.php.net/manual/en/set.mongodb.php

The driver is written to be a bare bone layer to talk to MongoDB, and
therefore misses many convenience features. Instead, these convenience methods
have been split out into a layer written in PHP, the `MongoDB Library`_.
Using this library should be your preferred way of interacting with MongoDB.

Please note that the new HHVM and PHP drivers implement a **different API**
from the legacy driver at http://pecl.php.net/package/mongo; therefore
existing libraries that use the legacy driver (e.g. `Doctrine MongoDB's ODM`_)
will not work with the new drivers.

.. _`Doctrine MongoDB's ODM`: http://doctrine-mongodb-odm.readthedocs.org/en/latest/
.. _`MongoDB Library`: http://mongodb.github.io/mongo-php-library/

In the long run, we hope that userland packages will be built atop this driver
to implement various APIs (e.g. a BC layer for the existing driver, new fluent
interfaces), management utilities (for creating admin utilities and cluster
management applications), and other interesting libraries.

Documentation
-------------

- http://php.net/mongodb
- http://mongodb.github.io/mongo-php-driver/

Installation
------------

Please ensure, that before you install the extension, the following tools are
installed: autoconf, automake, and libtool.

Compiling this extension requires the ``hphpize`` command, which is available by
installing the ``hhvm-dev`` package from the
`HHVM repositories <https://github.com/facebook/hhvm/wiki/Prebuilt-Packages-for-HHVM>`_
or `building HHVM from source <https://github.com/facebook/hhvm/wiki/Building-and-Installing-HHVM>`_.

Besides the extension, we recommend using it in combination with our `userland
library <https://github.com/mongodb/mongo-php-library>`_ which
is distributed as `mongodb/mongodb
<https://packagist.org/packages/mongodb/mongodb>`_ for Composer.

From a package (.tgz)
~~~~~~~~~~~~~~~~~~~~~

Releases of this driver can be found on
https://github.com/mongodb/mongo-hhvm-driver/releases

Please download the latest .tgz file into a directory, and run the following
commands::

	mkdir hhvm-mongodb
	cd hhvm-mongodb
	tar -xvzf ../hhvm-mongodb-1.0alpha1.tgz
	cd hhvm-mongodb-1.0alpha1
	hphpize
	cmake .
	make configlib
	make -j [number_of_processor_cores] # eg. make -j 4
	make install
	
Building from source
~~~~~~~~~~~~~~~~~~~~

Run the following 
commands::

	git clone https://github.com/mongodb/mongo-hhvm-driver --branch master
	cd mongo-hhvm-driver/
	git submodule update --init
	hphpize
	cmake .
	make configlib
	make -j [number_of_processor_cores] # eg. make -j 4
	make install

Installing
----------

In your ``/etc/hhvm/php.ini``, add the following lines (adjusting paths if
necessary)::

	hhvm.dynamic_extensions[mongodb]=mongodb.so

Contributing
------------

See `CONTRIBUTING.rst <CONTRIBUTING.rst>`_.

Related Projects
----------------

- `PHP Implementation of this driver <https://github.com/mongodb/mongo-php-driver>`_
- `Official high-level library <https://github.com/mongodb/mongo-php-library>`_
