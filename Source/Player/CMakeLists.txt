# CMake build : Player

if (NOT DEFINED PLAYER_NAME)
    set(PLAYER_NAME "Player")
endif ()

set(PLAYER_VERSION ${CMAKE_PROJECT_VERSION})

set(PLAYER_SCREEN_WIDTH 640 CACHE STRING "Player screen width default value" FORCE)
set(PLAYER_SCREEN_HEIGHT 480 CACHE STRING "Player screen height default value" FORCE)
set(PLAYER_SCREEN_BPP 32 CACHE STRING "Player screen bpp default value" FORCE)

configure_file(config.h.in "${CMAKE_CURRENT_SOURCE_DIR}/config.h")

set(PLAYER_HEADERS
        Game.h
        GameConfig.h
        GameStack.h
        GamePlayer.h
        PlayerRegistry.h
        NeMoContext.h
        WinContext.h
        Splash.h
        CmdlineParser.h
        ErrorProtocol.h
        LogProtocol.h
        )

set(PLAYER_SOURCES
        Player.cpp
        Game.cpp
        GameConfig.cpp
        GameStack.cpp
        GamePlayer.cpp
        PlayerRegistry.cpp
        NeMoContext.cpp
        WinContext.cpp
        Splash.cpp
        CmdlineParser.cpp
        ErrorProtocol.cpp
        LogProtocol.cpp
        Player.rc
        )

add_executable(Player ${PLAYER_HEADERS} ${PLAYER_SOURCES})
set_target_properties(Player PROPERTIES
        OUTPUT_NAME ${PLAYER_NAME}
        VERSION ${PLAYER_VERSION}
        WIN32_EXECUTABLE TRUE
        )

target_include_directories(Player PRIVATE
        ${BUILDING_BLOCKS_SOURCE_DIR}
        )

target_link_libraries(Player PRIVATE CK2 VxMath)

install(TARGETS Player RUNTIME DESTINATION Bin)