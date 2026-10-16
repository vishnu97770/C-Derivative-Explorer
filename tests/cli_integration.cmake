# Run with: cmake -DEXE=... -DOUT_DIR=... -P cli_integration.cmake
file(MAKE_DIRECTORY ${OUT_DIR})

# expect_rc(<code> <label> args...): run the CLI, require an exact exit code.
function(expect_rc code label)
  execute_process(COMMAND ${EXE} ${ARGN} RESULT_VARIABLE rc OUTPUT_VARIABLE out ERROR_VARIABLE err)
  if(NOT rc EQUAL ${code})
    message(FATAL_ERROR "${label}: expected exit ${code}, got ${rc}\n${out}\n${err}")
  endif()
  set(LAST_OUT "${out}" PARENT_SCOPE)
  set(LAST_ERR "${err}" PARENT_SCOPE)
endfunction()
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

# --- exit-code policy ------------------------------------------------------------------
# Partial success (>= 1 ok row) stays 0, even though some rows are non-ok.
expect_rc(0 "partial success" --function log --x 0.005 --steps 0.01 --methods forward,central)
if(NOT LAST_OUT MATCHES "invalid_domain")
  message(FATAL_ERROR "partial run should report the invalid_domain row:\n${LAST_OUT}")
endif()

# Every row non-ok -> exit 3; CSV is still written and rows carry the reason.
set(CSV3 ${OUT_DIR}/all_bad.csv)
file(REMOVE ${CSV3})
expect_rc(3 "all rows non-ok (overflow)" --function exp --x 1000 --step 0.01 --csv ${CSV3})
file(READ ${CSV3} bad_csv)
if(NOT bad_csv MATCHES "non_finite")
  message(FATAL_ERROR "all-bad CSV should contain non_finite rows:\n${bad_csv}")
endif()
expect_rc(3 "all rows degenerate" --function sin --x 1 --steps 1e-17,1e-16)
if(NOT LAST_OUT MATCHES "degenerate_step")
  message(FATAL_ERROR "terminal output should show degenerate_step:\n${LAST_OUT}")
endif()
# Degenerate plus a usable step: still a successful experiment.
set(CSVD ${OUT_DIR}/mixed.csv)
file(REMOVE ${CSVD})
expect_rc(0 "degenerate + ok" --function sin --x 1 --steps 1e-1,1e-17 --csv ${CSVD})
file(READ ${CSVD} mixed_csv)
if(NOT mixed_csv MATCHES "degenerate_step")
  message(FATAL_ERROR "mixed CSV should contain degenerate_step:\n${mixed_csv}")
endif()
# CSV output failure keeps its own code even when every row is also non-ok.
expect_rc(1 "csv failure" --function sin --x 1 --step 0.1 --csv /nonexistent_dir_xyz/o.csv)
expect_rc(1 "csv failure beats all-non-ok" --function exp --x 1000 --step 0.01
          --csv /nonexistent_dir_xyz/o.csv)
# Malformed argument -> 2.
expect_rc(2 "malformed step" --function sin --x 1 --step abc)

# --- --log-steps validation (B2) --------------------------------------------------------
foreach(bad "1:2:1.5" "1:2:0" "1:2:-3" "1:2:abc" "1:2:" "1:2:+3" "1:2:1e2"
            "1:2:99999999999999999999" "1:2:100001" "1:2:3:4" "2:1" "1:nan" "1:1e9" "1")
  expect_rc(2 "log-steps ${bad}" --function sin --x 1 --log-steps ${bad})
endforeach()
expect_rc(0 "log-steps default N" --function sin --x 1 --methods central --log-steps 1:3)
if(NOT LAST_OUT MATCHES "3 results")
  message(FATAL_ERROR "1:3 should give 3 steps:\n${LAST_OUT}")
endif()
expect_rc(0 "log-steps explicit N" --function sin --x 1 --methods central --log-steps 1:3:5)
if(NOT LAST_OUT MATCHES "5 results")
  message(FATAL_ERROR "1:3:5 should give 5 steps:\n${LAST_OUT}")
endif()
expect_rc(0 "log-steps single" --function sin --x 1 --methods central --log-steps 2:2:1)
if(NOT LAST_OUT MATCHES "1 results")
  message(FATAL_ERROR "2:2:1 should give 1 step:\n${LAST_OUT}")
endif()
