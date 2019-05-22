#!/bin/bash

cd libs/elogger/build/
rm -r ../build-gcc/*
rm -r *
cmake -DCMAKE_TOOLCHAIN_FILE=../../cmake/buildroot.cmake ..
make

cd ../../libini/build/
rm -r ../build-gcc/*
rm -r *
cmake -DCMAKE_TOOLCHAIN_FILE=../../cmake/buildroot.cmake ..
make

cd ../../../apps/can_worker
./build.sh

cd ../../apps/udp_worker
./build.sh
