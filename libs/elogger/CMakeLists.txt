cmake_minimum_required (VERSION 2.8)
PROJECT(elogger)

if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++0x")
endif()

INCLUDE_DIRECTORIES(.)
INCLUDE_DIRECTORIES(../../common/include)

ADD_LIBRARY( elogger SHARED
elogger.cpp
elogger.h
iloghandler.h
consoleloghandler.cpp
consoleloghandler.h
fileloghandler.cpp
fileloghandler.h
sysloghandler.cpp
sysloghandler.h
../../common/include/defines.h
)
