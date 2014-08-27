cmake_minimum_required(VERSION 2.8)

if (NOT DEFINED DynamoRIO_DIR)
  set(DynamoRIO_DIR "${PROJECT_SOURCE_DIR}/dynamorio/cmake" CACHE PATH
      "DynamoRIO installation's cmake directory")
endif (NOT DEFINED DynamoRIO_DIR)

if (NOT DEFINED DrMemoryFramework_DIR)
  set(DrMemoryFramework_DIR "${PROJECT_SOURCE_DIR}/drmemory/drmf" CACHE PATH
      "DrMemory Framework directory")
endif (NOT DEFINED DrMemoryFramework_DIR)

add_library(drthread SHARED drthread.c)
find_package(DynamoRIO)
find_package(DrMemoryFramework)

set(CMAKE_BUILD_TYPE "Debug")

if (NOT DynamoRIO_FOUND)
  message(FATAL_ERROR "DynamoRIO package required to build")
endif(NOT DynamoRIO_FOUND)
configure_DynamoRIO_client(drthread)
use_DynamoRIO_extension(drthread drwrap)
use_DynamoRIO_extension(drthread drsyms)
use_DynamoRIO_extension(drthread umbra)

execute_process(COMMAND ${CMAKE_COMMAND} -E
    create_symlink ${PROJECT_SOURCE_DIR}/dynamorio ${PROJECT_BINARY_DIR}/dynamorio)

set(DR_PATH "dynamorio")
get_property(DRTHREAD_PATH TARGET drthread PROPERTY LOCATION)
get_property(UMBRA_PATH TARGET umbra PROPERTY LOCATION)

FILE(COPY "${UMBRA_PATH}" DESTINATION "${PROJECT_BINARY_DIR}")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(ARCH 64)
else()
    set(ARCH 32)
endif()

configure_file("drthread.sh" "${PROJECT_BINARY_DIR}/drthread.sh")

# Tests
enable_testing()
add_subdirectory(tests)
configure_file(tests/runtest.cmake ${CMAKE_CURRENT_BINARY_DIR}/tests/runtest.cmake COPYONLY)
