#!/bin/bash

cd libs/elogger/build-gcc/
rm -r *
rm -r ../build/*
cmake ..
make

cd ../../libini/build-gcc/
rm -r *
rm -r ../build/*
cmake ..
make

cd ../../../apps/can_worker
./build.sh HOST

cd ../../apps/udp_worker
./build.sh HOST
