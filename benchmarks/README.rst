Benchmarks
==========

How to run these tests?

Set-up
------

First of all, you need to set a few things up:

- Install the PHP Library through composer:

  ``php ~/composer.phar install``

- Extract the datasets:

  ``cd data; tar -xvjf data.tar.bz2; cd -``

Start MongoDB
-------------

For all tests to run, you need a running MongoDB. To be comparable, you need
to start it up with the inMemory engine, and no journal. The inMemory engine
only works with an Enterprise build. If you don't have that, use the
``mmapv1`` storage engine instead.

My ``/etc/mongod-benchmark.conf`` file contains (only)::

	storage:
	  dbPath: /storage2/mongodb/inMemory
	  journal:
	    enabled: false
	  engine:
	    inMemory
	
	systemLog:
	  verbosity: 0
	  quiet: true
	  destination: file
	  logAppend: true
	  path: /var/log/mongodb/mongod-benchmark.log

Then start MongoDB with: ``sudo ./mongod --config
/etc/mongod-benchmark.conf``.

Running Tests with PHP
----------------------

First of all, make sure you have a PHP that is **not** compiled in
debugging mode. You also should have ``opcache`` loaded, or specified when
you run the tests. I tend to run it like this, so prevent other stuff from
creeping in::

	php -n -dzend_extension=opcache.so -dopcache.enable_cli=1 -dextension=mongodb.so runner.php

Running Tests with HHVM
-----------------------

First of all, make sure that you don't have a debug build, as it is a
**lot** slower. To run, use::

	hhvm -vDynamicExtensions.0=/home/derick/dev/php/mongodb-mongo-hhvm-driver/mongodb.so runner.php
