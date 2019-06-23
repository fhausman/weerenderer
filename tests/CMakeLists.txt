project(renderer_tests)
cmake_minimum_required(VERSION 3.1)

set(SOURCE_FILES tests.cpp ../renderer.cpp ../img/tgaimage.cpp ../img.cpp)
set(HEADER_FILES ../img/tgaimage.h ../img.hpp)

add_executable(renderer_tests ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(renderer_tests PRIVATE Catch2/single_include/catch2)

set_property(TARGET renderer_tests PROPERTY CXX_STANDARD 17)
