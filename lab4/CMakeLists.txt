cmake_minimum_required(VERSION 3.14)
project(MessageQueue)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(receiver
  receiver.cpp
  message_queue.cpp
  message_queue.h
)

add_executable(sender
  sender.cpp
  message_queue.cpp
  message_queue.h
)

add_executable(tests
  tests.cpp
  message_queue.cpp
  message_queue.h
)
target_link_libraries(tests gtest gtest_main)

enable_testing()
add_test(NAME unit_tests COMMAND tests)
