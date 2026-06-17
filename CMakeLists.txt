cmake_minimum_required(VERSION 3.14)
project(LazySequenceLab)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

add_library(lab_lib STATIC
    src/SubstringFrequencyCounter.cpp
)

target_include_directories(lab_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

enable_testing()
add_executable(run_tests
    src/test_main.cpp
)
target_link_libraries(run_tests PRIVATE lab_lib GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(run_tests)