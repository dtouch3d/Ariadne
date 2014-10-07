cmake_minimum_required(VERSION 2.8)

if (NOT DEFINED DynamoRIO_DIR)
  set(DynamoRIO_DIR "${PROJECT_SOURCE_DIR}/dynamorio/cmake" CACHE PATH
      "DynamoRIO installation's cmake directory")
endif (NOT DEFINED DynamoRIO_DIR)

if (NOT DEFINED DrMemoryFramework_DIR)
  set(DrMemoryFramework_DIR "${PROJECT_SOURCE_DIR}/dynamorio/drmemory/drmf" CACHE PATH
      "DrMemory Framework directory")
endif (NOT DEFINED DrMemoryFramework_DIR)

add_library(ariadne SHARED ariadne.c)
find_package(DynamoRIO)
find_package(DrMemoryFramework)

#set(CMAKE_BUILD_TYPE "Debug")

if (NOT DynamoRIO_FOUND)
  message(FATAL_ERROR "DynamoRIO package required to build")
endif(NOT DynamoRIO_FOUND)
configure_DynamoRIO_client(ariadne)
use_DynamoRIO_extension(ariadne umbra)
use_DynamoRIO_extension(ariadne drwrap)
use_DynamoRIO_extension(ariadne drsyms)
use_DynamoRIO_extension(ariadne drmgr)
use_DynamoRIO_extension(ariadne drcontainers)

execute_process(COMMAND ${CMAKE_COMMAND} -E
    create_symlink ${PROJECT_SOURCE_DIR}/dynamorio ${PROJECT_BINARY_DIR}/dynamorio)

set(DR_PATH "dynamorio")
get_property(DRTHREAD_PATH TARGET ariadne PROPERTY LOCATION)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(ARCH 64)
else()
    set(ARCH 32)
endif()

configure_file("ariadne.sh" "${PROJECT_BINARY_DIR}/ariadne.sh")

# Tests
enable_testing()
add_subdirectory(tests)
configure_file(tests/runtest.cmake ${CMAKE_CURRENT_BINARY_DIR}/tests/runtest.cmake COPYONLY)
