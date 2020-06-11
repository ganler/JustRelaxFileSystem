cmake_minimum_required(VERSION 3.15)
project(JustRelaxFileSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

ADD_LIBRARY(JRFS_LIB JRFS/JRFS.cpp JRFS/JRFS.hpp JRFS/api.hpp JRFS/api.cpp)

add_executable(jrfs-cli cli.cpp)
target_link_libraries(jrfs-cli JRFS_LIB)