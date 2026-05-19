cmake_minimum_required(VERSION 3.14)
project(Lab5 LANGUAGES CXX)

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
    file_utils.cpp
    lock_utils.cpp
    pipe_utils.cpp
    proc_utils.cpp
)

target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/utils)

add_executable(Server
    Server.cpp
)
target_link_libraries(Server PRIVATE utils)

add_executable(Client
    Client.cpp
)
target_link_libraries(Client PRIVATE utils)

add_executable(Lab5Tests
    tests/test_file_utils.cpp
    tests/test_file_utils_extra.cpp
    tests/test_lock_utils.cpp
    tests/test_lock_utils_concurrency.cpp
    tests/test_pipe_utils.cpp
    tests/test_pipe_connect_timeout.cpp
    tests/test_proc_utils.cpp
    tests/test_integration_simulated.cpp
)

target_include_directories(Lab5Tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/utils)
target_link_libraries(Lab5Tests
    PRIVATE
        utils
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(Lab5Tests)
