========
Tutorial
========

In this tutorial you will learn how to set-up NGINX with HHVM and MongoDB.
However, this tutorial is not meant to be an all inclusive guide on how ito
run NGINX and HHVM in a production environment.

After the initial set-up, we continue explaining how to get started with the
MongoDB driver and library for HHVM to write our first project.

Installation
============

In this tutorial, we will be using a Debian system, with NGINX installed
through ``apt-get`` and HHVM installed from **source** into
``/usr/local/hhvm/3.9.1`` (with the binary being at
``/usr/local/hhvm/3.9.1/bin/hhvm``). 

NGINX
-----

We simply install NGINX by running ``apt-get install nginx-full``. If this
fails to start, it is likely that you already have Apache running on the same
port. If that happens, you will see the following lines in
``/var/log/nginx/error.log``::

	2015/09/29 10:19:27 [emerg] 22445#22445: bind() to 0.0.0.0:80 failed (98:Address already in use)
	2015/09/29 10:19:27 [emerg] 22445#22445: bind() to [::]:80 failed (98: Address already in use)

If this happens, simple remove Apache by running ``apt-get remove apache2``.

HHVM
----

I have installed HHVM from a source build, mostly because I needed one with
some of my own patches and debug symbols. The folks at Facebook also provide
pre-built packages, which is probably what you *want* to use in production and
development. You can find them at the `HHVM Wiki`_.

.. _`HHVM Wiki`: https://github.com/facebook/hhvm/wiki/Prebuilt-Packages-on-Debian-7

You need to install the ``hhvm`` **and** ``hhvm-dev`` packages. The latter is
needed so that we can compile the MongoDB HHVM extension later on.

Because I installed from source, I had to do some extra work. I had to create
``/var/run/hhvm``:

 - ``sudo mkdir -p /var/run/hhvm``
 - ``sudo chown www-data.www-data /var/run/hhvm``
 - ``sudo mkdir /etc/hhvm``
 - ``sudo touch /etc/hhvm/php.ini``
 - ``sudo chown derick /etc/hhvm/php.ini`` (So that you don't have to ``sudo``
   to edit the file)
 - ``echo "date.timezone=Europe/London" >> /etc/hhvm/php.ini`` (To see whether
   it actually works)

I also had to start HHVM as ``www-data`` user. For now, I am running it in the
foreground in *server* mode as follows::

	sudo -u www-data -s /usr/local/hhvm/3.9.1/bin/hhvm \
		--mode server \
		-vServer.Type=fastcgi \
		-vServer.FileSocket=/var/run/hhvm/sock

Making NGINX talk to HHVM
-------------------------

Once HHVM runs, we need to tell NGINX how to talk to HHVM for ``.php``
files. Although this is perhaps not the most clean way of doing this, you
can add the following snippet to ``/etc/nginx/sites-enabled/default``, just
after the ``location / {`` … ``}`` section::

	location ~ \.php$ {
		fastcgi_pass unix:/var/run/hhvm/sock;
		fastcgi_index index.php;
		fastcgi_param SCRIPT_FILENAME $document_root$fastcgi_script_name;
		include fastcgi_params;
	}

After adding the snippet, you should restart NGINX::

	sudo service nginx restart

Just to see that it all works now, we will create a project directory and in
there, we place a ``index.php`` file with a ``phpinfo()`` file:

- Create the project directory: ``sudo mkdir -p  /var/www/html/my-first-project``
- Change permissions to your user: ``sudo chown derick.www-data /var/www/html/my-first-project``
- Create the file ``/var/www/html/my-first-project/index.php``. From now on,
  I will **not** include the full path ``/var/www/html/my-first-project/``
  when I mention file names. Put the following content in this file::

		<?php
		phpinfo();
		?>

Now in your browser, request the page
``http://gargleblaster/my-first-project/index.php`` (but adjust the
hostname). This should then show a page starting with "HHVM Version 3.9.1"
followed by several tables with information.


MongoDB Driver for HHVM
-----------------------

The MongoDB driver is the part that links up the PHP in HHVM with the database
server. To install and register the driver with HHVM, you need to take the
following steps:

- Download the latest driver from
  https://github.com/mongodb-labs/mongo-hhvm-driver-prototype/releases. At
  the moment, there is only `hhvm-mongodb-1.0alpha1.tgz`_ so we will be using
  this one in the examples.
- Unpack the archive: ``tar -xvzf hhvm-mongodb-1.0alpha1.tgz``
- ``cd`` into the newly created directory: ``cd hhvm-mongodb-1.0alpha1``
- Generate the configure files for the bundled libraries. For this to work,
  you need to have the ``automake``, ``autoconf`` and ``libtool`` packages
  installed (through ``apt-get``).

  - ``cd libbson; ./autogen.sh; cd ..``
  - ``cd libmongoc; ./autogen.sh; cd ..``

- Generate the Makefile: ``hphpize && cmake .``
- Build the driver: ``make``
- Install the driver: ``sudo make install``. This last step tells you were it
  has installed the binary file ``mongodb.so``. In my case, it showed:
  ``Installing: /usr/local/hhvm/3.9.1/lib/hhvm/extensions/20150212/mongodb.so``

Now the driver is installed, you need to enable it in HHVM. In order to do
this, you need to add the following lines to ``/etc/hhvm/php.ini``, swapping
out my directory name for the that showed when running ``make install``::

	hhvm.dynamic_extension_path=/usr/local/hhvm/3.9.1/lib/hhvm/extensions/20150212
	hhvm.dynamic_extensions[mongodb]=mongodb.so

After you have done that, you need to stop HHVM by pressing Ctrl-C in the
shell running HHVM, and then start it again as above::

	sudo -u www-data -s /usr/local/hhvm/3.9.1/bin/hhvm \
		--mode server \
		-vServer.Type=fastcgi \
		-vServer.FileSocket=/var/run/hhvm/sock

.. _`hhvm-mongodb-1.0alpha1.tgz`: https://github.com/mongodb-labs/mongo-hhvm-driver-prototype/releases/download/1.0alpha1/hhvm-mongodb-1.0alpha1.tgz

In order to test that it works, we edit our ``index.php`` file, and replace
its contents with::

	<?php
	var_dump(phpversion("mongodb"));
	?>

This should output something like::

	string(9) "1.0alpha1"


The driver implements the same API as its PHP variant, and
documentation can therefore be found in the `PHP Documentation`_.

.. _`PHP Documentation`: http://docs.php.net/manual/en/set.mongodb.php

PHP Library (PHPLIB)
--------------------

The last thing we still need to install to get started on the application
itself, is the PHP library.

The library needs to be installed with composer. In your project directory
(``/var/www/html/my-first-project``) type:

- ``curl -sS https://getcomposer.org/installer -o installer.php``
- ``hhvm installer.php``
- ``rm installer.php``

This downloads and installs composer. Wherever it says "Use it: php
composer.phar", it of course means ``hhvm composer.phar``.

With composer installed, we can now install the library::

	hhvm composer.phar require mongodb/mongodb

It outputs something akin to::

	Using version ^0.2.0 for mongodb/mongodb
	./composer.json has been created
	Loading composer repositories with package information
	Updating dependencies (including require-dev)
	  - Installing mongodb/mongodb (0.2.0)
		Downloading: 100%         

	Writing lock file
	Generating autoload files

And it has created several files (``composer.json``, ``composer.lock``) as
well as the ``vendor`` directory that contains the library.

Using the Library
-----------------

Composer manages your dependencies, and will provide you with a loader that
you include with the following at the start of your script::

	<?php
	require 'vendor/autoload.php';

With this done, you can now use any of the functionality as described in the
documentation_.

If you are familiar with the old driver, it should look too much out of place.
The only big difference is that the Database class is only used for Database
specific operations, and no longer to "obtain" a Collection handler. The CRUD_
operations on the Collection class are also renamed for clarity, and to be in
accordance with a new language-agnostic specification_.

As an example, this is how you insert a document into the *beers* collection
of the *demo* database::

	<?php
	require 'vendor/autoload.php'; // include composer goodies

	$manager = new MongoDB\Driver\Manager("mongodb://localhost:27017");
	$collection = new MongoDB\Collection($manager, "demo.beers");

	$result = $collection->insertOne( [ 'name' => 'Hinterland', 'brewery' => 'BrewDog' ] );

	echo "Inserted with Object ID '{$result->getInsertedId()}'";
	?>

Instead of the original document being modified to add the newly generated
``_id`` field, this is now part of the result that comes back from the
``insertOne`` method.

After insertion, you can of course also query the data that you have just
inserted. For that, you use the ``find`` method which returns a cursor that
you can iterate over::

	<?php
	require 'vendor/autoload.php'; // include composer goodies

	$manager = new MongoDB\Driver\Manager("mongodb://localhost:27017");
	$collection = new MongoDB\Collection($manager, "demo.beers");

	$result = $collection->find( [ 'name' => 'Hinterland', 'brewery' => 'BrewDog' ] );

	foreach ($result as $entry)
	{
		echo $entry->_id, ': ', $entry->name, "\n";
	}
	?>

You might have noticed that instead of accessing the ``_id`` and ``name``
fields is no longer done through an array access operator. Instead, they are
now properties of a ``stdClass`` object. You can find more information on how
serialisation and deserialisation between PHP variables and the BSON stored in
MongoDB in the `persistence`_ specification. 

.. _documentation: http://mongodb-labs.github.io/mongo-php-library-prototype/api
.. _persistence: https://github.com/mongodb-labs/mongo-hhvm-driver-prototype/blob/master/serialization.rst
