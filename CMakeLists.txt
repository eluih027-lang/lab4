cmake_minimum_required(VERSION 3.14)

project(Lab4LazyStreamsSubstringFrequency LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(lab4
    src/main.cpp
    src/SubstringFrequencyCounter.cpp
)

target_include_directories(lab4 PRIVATE src)
