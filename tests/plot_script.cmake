file(MAKE_DIRECTORY ${OUT_DIR})
# Empty file (header only) and missing columns must fail with nonzero exit.
file(WRITE ${OUT_DIR}/empty.csv "schema_version,function_id,method_id,x,h,absolute_error,status\n")
file(WRITE ${OUT_DIR}/missing.csv "a,b\n1,2\n")
foreach(f empty missing)
  execute_process(COMMAND ${PY} -I ${SCRIPT} ${OUT_DIR}/${f}.csv ${OUT_DIR}/${f}.png
                  RESULT_VARIABLE rc OUTPUT_QUIET ERROR_QUIET)
  if(rc EQUAL 0)
    message(FATAL_ERROR "${f}.csv should be rejected")
  endif()
endforeach()
