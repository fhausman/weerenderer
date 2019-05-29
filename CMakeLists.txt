project(renderer)
cmake_minimum_required(VERSION 3.1)

set(SOURCE_FILES
    renderer.cpp
    img/tgaimage.cpp)

set(HEADER_FILES
    img/tgaimage.h
    hola/hola.hpp)

add_subdirectory(tinyobjloader)
add_executable(renderer ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(renderer tinyobjloader)
set_property(TARGET renderer PROPERTY CXX_STANDARD 17)
