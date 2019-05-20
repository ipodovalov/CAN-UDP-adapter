#!/bin/bash

cd libs/elogger/build-gcc/
cmake ..
make

cd ../../libini/build-gcc/
cmake ..
make

cd ../../../apps/can_worker
./build.sh HOST

cd ../../apps/udp_worker
./build.sh HOST
