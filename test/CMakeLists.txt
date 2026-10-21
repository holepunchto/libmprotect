list(APPEND tests
  alloc
  alloc-empty
  alloc-twice
  fault-noaccess-read
  fault-overflow
  fault-readonly-write
  freed
)

foreach(test IN LISTS tests)
  add_executable(${test} ${test}.c)

  target_link_libraries(
    ${test}
    PRIVATE
      mprotect_static
  )

  add_test(
    NAME ${test}
    COMMAND ${test}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )

  set_tests_properties(
    ${test}
    PROPERTIES
    TIMEOUT 30
    SKIP_RETURN_CODE 77
  )
endforeach()
