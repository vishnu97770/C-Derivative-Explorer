# Run with: cmake -DEXE=... -DOUT_DIR=... -P cli_integration.cmake
file(MAKE_DIRECTORY ${OUT_DIR})
set(CSV ${OUT_DIR}/out.csv)
file(REMOVE ${CSV})

execute_process(COMMAND ${EXE} --function sin --x 1 --methods forward,central
                --steps 0.1,0.01 --csv ${CSV} RESULT_VARIABLE rc OUTPUT_QUIET)
if(NOT rc EQUAL 0)
  message(FATAL_ERROR "valid run exited ${rc}")
endif()
file(STRINGS ${CSV} lines)
list(LENGTH lines n)
if(NOT n EQUAL 5)
  message(FATAL_ERROR "expected header + 4 rows, got ${n}")
endif()
list(GET lines 0 header)
if(NOT header MATCHES "^schema_version,function_id,method_id,x,h,estimate")
  message(FATAL_ERROR "bad header: ${header}")
endif()

execute_process(COMMAND ${EXE} --function nope --x 1 --step 0.1 RESULT_VARIABLE rc
                OUTPUT_QUIET ERROR_QUIET)
if(rc EQUAL 0)
  message(FATAL_ERROR "unknown function should fail")
endif()

execute_process(COMMAND ${EXE} --function sin --x 1 --step 0.1 --csv /nonexistent_dir_xyz/o.csv
                RESULT_VARIABLE rc OUTPUT_QUIET ERROR_QUIET)
if(rc EQUAL 0)
  message(FATAL_ERROR "unwritable CSV path should give nonzero exit")
endif()

execute_process(COMMAND ${EXE} --function sin --x 1 --step abc RESULT_VARIABLE rc
                OUTPUT_QUIET ERROR_QUIET)
if(rc EQUAL 0)
  message(FATAL_ERROR "bad step should fail")
endif()
