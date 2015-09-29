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
needed so that we can compile the HHVM extension later on.

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

.. _`hhvm-mongodb-1.0alpha1.tgz`: https://github.com/mongodb-labs/mongo-hhvm-driver-prototype/releases/download/1.0alpha1/hhvm-mongodb-1.0alpha1.tgz

The driver implements the same API as its PHP variant, and
documentation can therefore be found in the `PHP Documentation`_.

.. _`PHP Documentation`: http://docs.php.net/manual/en/set.mongodb.php
