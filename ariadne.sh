#!/bin/bash

# Variables are replaces with their proper values at
# configure time by cmake.

DR_OPS=

${PROJECT_BINARY_DIR}/dynamorio/bin@ARCH@/drrun -debug -native_exec_list libpthread.so -native_exec_retakeover -native_exec_opt -c @DRTHREAD_PATH@ -- $@
