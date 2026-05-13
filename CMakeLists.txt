cmake_minimum_required(VERSION 3.16)

project(Ballanced
        VERSION 0.1.0
        DESCRIPTION "Ballance Decompilation"
        HOMEPAGE_URL https://github.com/doyaGu/Ballanced
        LANGUAGES C CXX
)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(GNUInstallDirs)
include(BallanceCache)
include(BallanceProject)
include(BallanceOptions)
include(BallanceCompiler)
include(BallanceVersion)

set(BALLANCE_PLAYER_VERSION_OVERRIDE "${BALLANCE_RUNTIME_VERSION}")
set(BALLANCE_MODULE_VERSION_OVERRIDE "${BALLANCE_RUNTIME_VERSION}")

add_subdirectory(Source)

include(BallanceStage)
