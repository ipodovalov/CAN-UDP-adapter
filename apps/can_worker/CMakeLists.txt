project(can_worker)
cmake_minimum_required(VERSION 2.8)

if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-comment -Wno-unused-variable -Wno-sign-compare -std=gnu++0x -Wno-unused-but-set-variable -Wno-misleading-indentation")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../common/include)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libs/elogger)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libs/elogger/build)

aux_source_directory(. SRC_LIST)
list (APPEND SRC_LIST "${CMAKE_CURRENT_SOURCE_DIR}/../common/src/embtime.c")

add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(${PROJECT_NAME} elogger)

target_link_libraries(${PROJECT_NAME} pthread)

target_link_libraries(${PROJECT_NAME} rt)
