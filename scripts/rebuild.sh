#!/bin/bash

hphpize
cmake -DCMAKE_C_FLAGS="-O0 -ggdb3" -DCMAKE_CXX_FLAGS="-O0 -ggdb3" -DCMAKE_BUILD_TYPE=Debug .
make clean
make
