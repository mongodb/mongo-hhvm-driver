MongoDB
=======

This is a proof-of-concept HHVM extension mimicking the proof-of-concept PHP
extension "phongo".

## Installation

Compiling this extension requires the `hphpize` command, which is available by
installing the `hhvm-dev` package from the
[HHVM repositories](https://github.com/facebook/hhvm/wiki/Prebuilt-Packages-for-HHVM)
or [building HHVM from source](https://github.com/facebook/hhvm/wiki/Building-and-Installing-HHVM).

Start by cloning this repository and initializing and configuring the libbson
and libmongoc submodules:

```
$ git clone https://github.com/10gen-labs/mongo-hhvm-driver-prototype.git
$ cd mongo-hhvm-driver-prototype/
$ git submodule update --init --recursive
$ cd libbson/
$ ./autogen.sh
$ cd ../libmongoc/
$ ./autogen.sh
$ cd ..
```

From the project directory, configure and build the extension:

```
$ hphpize && cmake . && make
```

This process should leave you with a `mongodb.so` file in the project directory.
Copy this to your HHVM extensions directory (e.g. `/etc/hhvm/extensions`).

```
$ cp mongodb.so /etc/hhvm/extensions
```

Add the following to your HHVM configuration (e.g. `/etc/hhvm/php.ini`):

```
hhvm.dynamic_extension_path = /etc/hhvm/extensions
hhvm.dynamic_extensions[mongodb] = mongodb.so
```
