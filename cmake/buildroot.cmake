#cmake -DCMAKE_TOOLCHAIN_FILE=../../../cmake/buildroot.cmake ..

# this one is important
SET(CMAKE_SYSTEM_NAME Linux)
#this one not so much
SET(CMAKE_SYSTEM_VERSION 1)

# specify the cross compiler
SET(CMAKE_C_COMPILER /home/user/sdk/buildroot/output/host/opt/ext-toolchain/bin/arm-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER /home/user/sdk/buildroot/output/host/opt/ext-toolchain/bin/arm-linux-gnueabihf-g++)

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH  /home/user/sdk/buildroot/output/target/)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

#SET(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
#SET(CMAKE_EXE_LINKER_FLAGS -static)
