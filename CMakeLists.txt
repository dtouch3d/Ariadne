cmake_minimum_required(VERSION 2.8)

if (NOT DEFINED DynamoRIO_DIR)
  set(DynamoRIO_DIR "${PROJECT_SOURCE_DIR}/dynamorio/cmake" CACHE PATH
      "DynamoRIO installation's cmake directory")
  message(STATUS ${DynamoRIO_DIR})
endif (NOT DEFINED DynamoRIO_DIR)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" "-std=c99")

add_library(drthread SHARED drthread.c)
find_package(DynamoRIO)
if (NOT DynamoRIO_FOUND)
  message(FATAL_ERROR "DynamoRIO package required to build")
endif(NOT DynamoRIO_FOUND)
configure_DynamoRIO_client(drthread)
use_DynamoRIO_extension(drthread drwrap)
use_DynamoRIO_extension(drthread drsyms)

execute_process(COMMAND ${CMAKE_COMMAND} -E
    create_symlink ${PROJECT_SOURCE_DIR}/dynamorio ${PROJECT_BINARY_DIR}/dynamorio)

set(DR_PATH "dynamorio")
get_property(DRTHREAD_PATH TARGET drthread PROPERTY LOCATION)

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
