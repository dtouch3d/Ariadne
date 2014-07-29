# INPUTS:
#    TEST_PROG
#    SRCDIR
#

message(STATUS "${TEST_PROG} ::: ${SRCDIR}")

# TODO: Args ?
execute_process(COMMAND ${TEST_PROG}
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE err)

message(STATUS "${err}")
