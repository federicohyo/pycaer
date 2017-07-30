#!/bin/sh

# This script is to compile library

# Compile libcaer
cd libcaer
mkdir build
cmake -DCMAKE_INSTALL_PREFIX=./build .
make
make install
cd ..

# Compile pycaer
PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$PWD/libcaer/build/lib/pkgconfig
mkdir build
cd build
cmake ..
make
