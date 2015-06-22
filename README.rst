MongoDB
=======

This is a proof-of-concept HHVM extension mimicking the proof-of-concept PHP
extension "phongo".

Building
--------

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

Running
-------

To run the rests::

	export TEST_PHP_EXECUTABLE=`which hhvm`
	hhvm -vDynamicExtensions=mongodb.so run-tests.php
