cmake_minimum_required(VERSION 3.14)
project(ThreadArray)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)

FetchContent_MakeAvailable(googletest)

add_executable(main_app
  main.cpp
  array_processor.cpp
  array_processor.h
)

add_executable(tests
  tests.cpp
  array_processor.cpp
  array_processor.h
)

target_link_libraries(tests gtest gtest_main)

enable_testing()
add_test(NAME unit_tests COMMAND tests)
