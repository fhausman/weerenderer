project(renderer)
cmake_minimum_required(VERSION 3.1)

set(SOURCE_FILES
    main.cpp
    renderer.cpp
    tgaimpl.cpp
    objimpl.cpp
    img/tgaimage.cpp)

set(HEADER_FILES
    renderer.hpp
    img.hpp
    tgaimpl.hpp
    model.hpp
    objimpl.hpp
    img/tgaimage.h
    hola/hola.hpp)

add_subdirectory(tinyobjloader)
add_subdirectory(tests)
add_executable(renderer ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(renderer tinyobjloader)
set_property(TARGET renderer PROPERTY CXX_STANDARD 17)
