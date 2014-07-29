# INPUTS:
#    TEST_PROG
#    SRCDIR
#

message(STATUS "${TEST_PROG} ::: ${SRCDIR}")

# TODO: Args ?
execute_process(COMMAND ${TEST_PROG}
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output)

message(STATUS "${CMAKE_CURRENT_BINARY_DIR}")
