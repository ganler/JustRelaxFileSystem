CMAKE_MINIMUM_REQUIRED(VERSION 3.14.0 FATAL_ERROR)
PROJECT(JustRelaxFileSystem)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

FILE(GLOB_RECURSE JRFS_LIB_SOURCES JRFS/*pp)

MESSAGE(STATUS "Found JRFS Source Files: ${JRFS_LIB_SOURCES}")

ADD_LIBRARY(JRFS_LIB ${JRFS_LIB_SOURCES})

FIND_PACKAGE(gflags REQUIRED)
INCLUDE_DIRECTORIES(${GFLAGS_INCLUDE_DIRS})

ADD_EXECUTABLE(jrfs-cli cli.cpp)
TARGET_LINK_LIBRARIES(jrfs-cli JRFS_LIB ${GFLAGS_LIBRARIES})


ENABLE_TESTING()
FILE(GLOB_RECURSE JRFS_TESTS ${CMAKE_SOURCE_DIR}/tests/*.cpp)

include(FetchContent)

set(FETCHCONTENT_QUIET off)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(BUILD_GTEST ON CACHE BOOL "" FORCE)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.8.0
)

FetchContent_GetProperties(googletest)
FetchContent_MakeAvailable(googletest)
if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    MESSAGE(STATUS "Adding new directories: ${googletest_SOURCE_DIR} ${googletest_BINARY_DIR}")
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()

FOREACH(TEST_FULL_PATH ${JRFS_TESTS})
    MESSAGE(STATUS ">>> [TEST] TO BUILD ${TEST_FULL_PATH}")
    GET_FILENAME_COMPONENT(TEST_NAME ${TEST_FULL_PATH} NAME_WE)
    # ~ NAME_WE means filename without directory | longest extension ~ See more
    # details at
    # https://cmake.org/cmake/help/v3.0/command/get_filename_component.html

    SET(TEST_TAR test.${TEST_NAME})
    ADD_EXECUTABLE(${TEST_TAR} ${TEST_FULL_PATH})
    TARGET_LINK_LIBRARIES(${TEST_TAR} PRIVATE JRFS_LIB gtest gtest_main})
    SET_PROPERTY(TARGET ${TEST_TAR} PROPERTY COMPILE_FLAGS "")
    ADD_TEST(NAME ${TEST_TAR} COMMAND ${TEST_TAR})
ENDFOREACH()