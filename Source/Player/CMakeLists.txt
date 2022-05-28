# CMake build : Player

if(NOT DEFINED PLAYER_NAME)
	set(PLAYER_NAME "Player")
endif()

set(PLAYER_VERSION ${CMAKE_PROJECT_VERSION})

set(PLAYER_SCREEN_WIDTH 640 CACHE STRING "Player screen width default value" FORCE)
set(PLAYER_SCREEN_HEIGHT 480 CACHE STRING "Player screen height default value" FORCE)
set(PLAYER_SCREEN_BPP 16 CACHE STRING "Player screen bpp default value" FORCE)

configure_file(config.h.in "${CMAKE_CURRENT_SOURCE_DIR}/config.h")

set(PLAYER_HEADERS
	Game.h
	GameStack.h
	GamePlayer.h
	PlayerRegistry.h
	NeMoContext.h
	WinContext.h
	Splash.h
	ErrorProtocol.h
	LogProtocol.h
	getopt.h
)

set(PLAYER_SOURCES
	Player.cpp
	Game.cpp
	GameStack.cpp
	GamePlayer.cpp
	PlayerRegistry.cpp
	NeMoContext.cpp
	WinContext.cpp
	Splash.cpp
	ErrorProtocol.cpp
	LogProtocol.cpp
	getopt.cpp
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

target_link_libraries(Player PRIVATE
    ResDll
	VirtoolsSDK::CK2
	VirtoolsSDK::VxMath
)

install(TARGETS Player RUNTIME DESTINATION Bin)