function(add_test_ariadne test_name test_src)
    add_test(NAME ${test_name}
        COMMAND ${CMAKE_COMMAND}
        -DTEST_PROG=$<TARGET_FILE:${test_name}>
        -DSRCDIR=${CMAKE_CURRENT_SOURCE_DIR}
        -P ${CMAKE_CURRENT_BINARY_DIR}/runtest.cmake)
    configure_file("${PROJECT_SOURCE_DIR}/tests/${test_name}.expect"
                   "${PROJECT_BINARY_DIR}/tests/${test_name}.expect" COPYONLY)
endfunction(add_test_ariadne)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_C_FLAGS "-pthread")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "suite/tests" PARENT_SCOPE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "suite/tests" PARENT_SCOPE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "suite/tests" PARENT_SCOPE)

add_executable(hello hello.c)
add_test_ariadne(hello hello.c)

add_executable(malloc malloc.c)
add_test_ariadne(malloc malloc.c)

add_executable(join join.c)
add_test_ariadne(join join.c)

add_executable(race1 race1.c)
add_test_ariadne(race1 race1.c)
