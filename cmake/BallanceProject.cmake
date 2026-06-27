# Root-only project policy for the superproject.

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not allowed. Use: cmake -B build")
endif ()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (WIN32)
    set(CMAKE_USE_RELATIVE_PATHS ON)
    set(CMAKE_SUPPRESS_REGENERATION ON)
endif ()

get_property(_ballance_is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if (NOT _ballance_is_multi_config AND NOT CMAKE_BUILD_TYPE)
    message(STATUS "[Ballance] Setting build type to 'Release'")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif ()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/stage" CACHE PATH "Staging root" FORCE)
    message(STATUS "[Ballance] Install prefix: ${CMAKE_INSTALL_PREFIX}")
endif ()

if (APPLE)
    set(CMAKE_MACOSX_RPATH ON)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH OFF)
    set(CMAKE_INSTALL_RPATH "@loader_path/../Bin")
elseif (UNIX)
    set(CMAKE_INSTALL_RPATH "$ORIGIN;$ORIGIN/../Bin")
endif ()
