cmake_minimum_required(VERSION 3.17.0 FATAL_ERROR)

project(platformer-server VERSION "0.1.0" LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

include(${CMAKE_SOURCE_DIR}/cmake/conan.cmake)

if(NOT ${CMAKE_BUILD_TYPE})
    set(${CMAKE_BUILD_TYPE} Release)
endif()

conan_cmake_run(CONANFILE conanfile.txt
                SETTINGS compiler.libcxx=libstdc++11
                BASIC_SETUP
                BUILD missing)

add_executable(server ${CMAKE_SOURCE_DIR}/src/main.cpp)

if(("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU") OR ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"))
    target_compile_options(server PRIVATE -Wall)
    target_compile_options(server PRIVATE -Wextra)
elseif(DEFINED MSVC)
    target_compile_options(server PRIVATE /W3)
endif()

target_link_libraries(server ${CONAN_LIBS})
target_precompile_headers(server PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_features(server PRIVATE cxx_std_17)

set(
    MY_SOURCES
    src/common/config.cpp
    src/common/json.cpp
    src/common/time.cpp
    src/database/connection.cpp
    src/game/collision.cpp
    src/game/map.cpp
    src/game/world.cpp
        src/game/system/movement.cpp
        src/game/system/session.cpp
        src/game/system/sync.cpp
    src/network/handler.cpp
    src/network/metrics.cpp
    src/network/packet.cpp
    src/network/socket.cpp
    src/network/socket_listener.cpp
)

target_sources(server PRIVATE ${MY_SOURCES})
