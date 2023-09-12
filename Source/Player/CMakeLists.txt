# Player

if (NOT DEFINED PLAYER_NAME)
    set(PLAYER_NAME "Player")
endif ()

set(PLAYER_VERSION ${CMAKE_PROJECT_VERSION})

set(PLAYER_SCREEN_WIDTH 640 CACHE STRING "Player screen width default value" FORCE)
set(PLAYER_SCREEN_HEIGHT 480 CACHE STRING "Player screen height default value" FORCE)
set(PLAYER_SCREEN_BPP 16 CACHE STRING "Player screen bpp default value" FORCE)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)

set(PLAYER_HEADERS
        PlayerRegistry.h
        GamePlayer.h
        Game.h
        GameStack.h
        NeMoContext.h
        WinContext.h
        Splash.h
        ErrorProtocol.h
        LogProtocol.h

        config.h
        resource.h
)

set(PLAYER_SOURCES
        Player.cpp
        PlayerRegistry.cpp
        GamePlayer.cpp
        Game.cpp
        GameStack.cpp
        NeMoContext.cpp
        WinContext.cpp
        Splash.cpp
        AntiPiracy.cpp
        ErrorProtocol.cpp
        LogProtocol.cpp

        Player.rc
)

add_executable(${PLAYER_NAME} ${PLAYER_HEADERS} ${PLAYER_SOURCES})

set_target_properties(${PLAYER_NAME} PROPERTIES
        OUTPUT_NAME ${PLAYER_NAME}
        WIN32_EXECUTABLE TRUE
)

target_include_directories(${PLAYER_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/Source/BuildingBlocks/TT_InterfaceManager_RT
        ${PLAYER_INCLUDE_DIR}
)

target_link_libraries(${PLAYER_NAME} PRIVATE CK2 VxMath ResDll)

install(TARGETS ${PLAYER_NAME} RUNTIME DESTINATION Bin)