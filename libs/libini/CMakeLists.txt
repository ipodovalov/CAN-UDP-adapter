cmake_minimum_required (VERSION 2.8)
PROJECT(inireader)

if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++0x")
endif()

INCLUDE_DIRECTORIES(.)
INCLUDE_DIRECTORIES(../../common/include)

ADD_LIBRARY( inireader STATIC 
inireader.cpp
inireader.h
ini.h
ini.c
../../common/include/defines.h
)
