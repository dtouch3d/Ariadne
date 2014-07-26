cmake_minimum_required(VERSION 2.8)

if (NOT DEFINED DynamoRIO_DIR)
  set(DynamoRIO_DIR "${PROJECT_SOURCE_DIR}/dynamorio/cmake" CACHE PATH
      "DynamoRIO installation's cmake directory")
  message(STATUS ${DynamoRIO_DIR})
endif (NOT DEFINED DynamoRIO_DIR)

add_library(drthread SHARED drthread.c)
find_package(DynamoRIO)
if (NOT DynamoRIO_FOUND)
  message(FATAL_ERROR "DynamoRIO package required to build")
endif(NOT DynamoRIO_FOUND)
configure_DynamoRIO_client(drthread)
