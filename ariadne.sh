#!/bin/bash

# Variables are replaces with their proper values at
# configure time by cmake.

DR_OPS=

${PROJECT_BINARY_DIR}/dynamorio/bin@ARCH@/drrun -thread_private -c @DRTHREAD_PATH@ -- $@
