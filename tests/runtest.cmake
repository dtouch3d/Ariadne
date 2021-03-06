# INPUTS:
#    TEST_PROG
#    SRCDIR
#

message(STATUS "${TEST_PROG} ::: ${SRCDIR}")

# TODO: Args ?
execute_process(COMMAND ../ariadne.sh ${TEST_PROG}
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE err)

file(READ "${TEST_PROG}.expect" expect)

message(STATUS "expect :: ${expect}")
message(STATUS "output :: ${output}")

if(NOT "${output}" MATCHES "${expect}")
    message(FATAL_ERROR "Test ${TEST_PROG} Failed!")
endif()
