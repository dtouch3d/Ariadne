set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_C_FLAGS "-pthread")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "suite/tests" PARENT_SCOPE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "suite/tests" PARENT_SCOPE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "suite/tests" PARENT_SCOPE)

add_executable(hello hello.c)
add_test(hello hello.c)
