cmake_minimum_required(VERSION 3.20)
project(derivative_explorer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(DE_ENABLE_SANITIZERS "Build with AddressSanitizer and UBSan" OFF)

add_library(derivative_warnings INTERFACE)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(derivative_warnings INTERFACE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
  if(DE_ENABLE_SANITIZERS)
    target_compile_options(derivative_warnings INTERFACE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(derivative_warnings INTERFACE -fsanitize=address,undefined)
  endif()
endif()

add_library(derivative_core
  src/result.cpp
  src/function.cpp
  src/derivative_methods.cpp
  src/metrics.cpp
  src/comparison.cpp
  src/csv_writer.cpp)
target_include_directories(derivative_core PUBLIC include)
target_link_libraries(derivative_core PRIVATE derivative_warnings)

add_executable(derivative-explorer src/main.cpp)
target_link_libraries(derivative-explorer PRIVATE derivative_core derivative_warnings)

include(CTest)
if(BUILD_TESTING)
  include(FetchContent)
  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz
    URL_HASH SHA256=65fab701d9829d38cb77c14acdc431d2108bfdbf8979e40eb8ae567edf10b27c
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
  FetchContent_MakeAvailable(googletest)

  add_executable(derivative_tests
    tests/test_functions.cpp
    tests/test_methods.cpp
    tests/test_metrics.cpp
    tests/test_comparison.cpp
    tests/test_csv_writer.cpp)
  target_link_libraries(derivative_tests PRIVATE derivative_core derivative_warnings GTest::gtest_main)
  include(GoogleTest)
  gtest_discover_tests(derivative_tests)

  add_test(NAME cli_integration
    COMMAND ${CMAKE_COMMAND}
      -DEXE=$<TARGET_FILE:derivative-explorer>
      -DOUT_DIR=${CMAKE_BINARY_DIR}/cli_test
      -P ${CMAKE_SOURCE_DIR}/tests/cli_integration.cmake)
endif()

# Plot-script contract tests (need Python + matplotlib; skipped otherwise).
if(BUILD_TESTING)
  find_package(Python3 COMPONENTS Interpreter)
  if(Python3_FOUND)
    add_test(NAME plot_script_rejects_bad_input
      COMMAND ${CMAKE_COMMAND}
        -DPY=${Python3_EXECUTABLE}
        -DSCRIPT=${CMAKE_SOURCE_DIR}/scripts/plot_results.py
        -DOUT_DIR=${CMAKE_BINARY_DIR}/plot_test
        -P ${CMAKE_SOURCE_DIR}/tests/plot_script.cmake)
  endif()
endif()
