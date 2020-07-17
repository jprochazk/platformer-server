cmake_minimum_required(VERSION 3.17.0 FATAL_ERROR)

project(server VERSION "0.1.0" LANGUAGES CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_subdirectory(src)