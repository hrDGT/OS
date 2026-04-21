cmake_minimum_required(VERSION 3.14)
project(Lab4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_library(utils STATIC
    utils/message_queue.cpp
    utils/input_parsing.cpp
)

target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/utils)

add_executable(Receiver
    Receiver.cpp
)
target_link_libraries(Receiver PRIVATE utils)

add_executable(Sender
    Sender.cpp
)
target_link_libraries(Sender PRIVATE utils)

add_executable(Lab4Tests
    tests/input_parsing_test.cpp
    tests/message_test.cpp
)

target_include_directories(Lab4Tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/utils)
target_link_libraries(Lab4Tests
    PRIVATE
        utils
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(Lab4Tests)
