cmake_minimum_required(VERSION 3.14)
project(MarkerThreads)

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
  marker_processor.cpp
  marker_processor.h
)

add_executable(tests
  tests.cpp
  marker_processor.cpp
  marker_processor.h
)

target_link_libraries(tests gtest gtest_main)

enable_testing()
add_test(NAME unit_tests COMMAND tests)
