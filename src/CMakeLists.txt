
add_executable(server src/main.cpp)

if(("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU") OR ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"))
    target_compile_options(-Wall)
    target_compile_options(-Wextra)
elseif(DEFINED MSVC)
    target_compile_options(/W3)
endif()

set(
    MY_SOURCES
    main.cpp
        common/config.cpp
        common/json.cpp
        common/time.cpp
        database/connection.cpp
        game/collision.cpp
        game/map.cpp
        game/world.cpp
            game/system/movement.cpp
            game/system/session.cpp
            game/system/sync.cpp
        network/handler.cpp
        network/metrics.cpp
        network/packet.cpp
        network/socket.cpp
        network/socket_listener.cpp
)

target_precompile_headers(server PRIVATE src/pch.h)
target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_sources(server PRIVATE ${MY_SOURCES})