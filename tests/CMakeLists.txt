function(add_test_drthread test_name test_src)
    add_test(NAME ${test_name}
        COMMAND ${CMAKE_COMMAND}
        -DTEST_PROG=$<TARGET_FILE:${test_name}>
        -DSRCDIR=${CMAKE_CURRENT_SOURCE_DIR}
        -P ${CMAKE_CURRENT_BINARY_DIR}/runtest.cmake)
endfunction(add_test_drthread)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_C_FLAGS "-pthread")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "suite/tests" PARENT_SCOPE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "suite/tests" PARENT_SCOPE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "suite/tests" PARENT_SCOPE)

add_executable(hello hello.c)
add_test_drthread(hello hello.c)
