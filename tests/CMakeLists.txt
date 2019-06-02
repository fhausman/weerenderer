project(renderer_tests)
cmake_minimum_required(VERSION 3.1)

set(SOURCE_FILES tests.cpp)

set(HEADER_FILES Catch2/include/catch.hpp)

add_executable(renderer_tests ${SOURCE_FILES} ${HEADER_FILES})
set_property(TARGET renderer_tests PROPERTY CXX_STANDARD 17)
