project(obj)
cmake_minimum_required(VERSION 3.1)

add_library(obj STATIC obj.hpp obj.cpp)
set_property(TARGET obj PROPERTY CXX_STANDARD 17)
