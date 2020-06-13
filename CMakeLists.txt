cmake_minimum_required(VERSION 3.15)
project(JustRelaxFileSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB JRFS_LIB_SOURCES JRFS/details/*pp JRFS/util/*pp)

MESSAGE(STATUS "Found JRFS Source Files: ${JRFS_LIB_SOURCES}")

ADD_LIBRARY(JRFS_LIB ${JRFS_LIB_SOURCES})
ADD_LIBRARY(JRFS_API JRFS/filesystem.hpp JRFS/filesystem.cpp)

add_executable(jrfs-cli cli.cpp)

target_link_libraries(JRFS_API JRFS_LIB)

FIND_PACKAGE(gflags REQUIRED)

INCLUDE_DIRECTORIES(${GFLAGS_INCLUDE_DIRS})
target_link_libraries(jrfs-cli JRFS_LIB JRFS_API ${GFLAGS_LIBRARIES})