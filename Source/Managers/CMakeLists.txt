cmake_minimum_required(VERSION 3.16)

project(CKManagers
        VERSION 2.1.0.14
        DESCRIPTION "Virtools manager plugins"
        LANGUAGES C CXX
)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.21")
    set(CKMANAGERS_IS_TOP_LEVEL ${PROJECT_IS_TOP_LEVEL})
else ()
    if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(CKMANAGERS_IS_TOP_LEVEL ON)
    else ()
        set(CKMANAGERS_IS_TOP_LEVEL OFF)
    endif ()
endif ()

if (CKMANAGERS_IS_TOP_LEVEL AND CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds not allowed. Run CMake from a separate directory: cmake -B build")
endif ()

if (CKMANAGERS_IS_TOP_LEVEL)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")
    set(CMAKE_USE_RELATIVE_PATHS TRUE)
    set(CMAKE_SUPPRESS_REGENERATION TRUE)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if (NOT IS_MULTI_CONFIG AND NOT CMAKE_BUILD_TYPE)
        message(STATUS "[CKManagers] Setting build type to 'Release'")
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the build type" FORCE)
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
    endif ()

    if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/install" CACHE PATH "Installation directory" FORCE)
        message(STATUS "[CKManagers] Setting install directory to ${CMAKE_INSTALL_PREFIX}")
    endif ()
endif ()

option(CKMANAGERS_BUILD_ALL "Build all available manager plugins" ON)
option(CKMANAGERS_BUILD_SDLINPUT "Build SdlInputManager" ON)
option(CKMANAGERS_BUILD_SDLSOUND "Build SdlSoundManager" ON)
option(CKMANAGERS_BUILD_PARAMETEROPERATIONS "Build ParameterOperations" ON)

if (NOT TARGET SDL3::SDL3)
    find_package(SDL3 CONFIG REQUIRED)
endif ()

set(CKMANAGERS_COMPONENTS "")

if (CKMANAGERS_BUILD_ALL OR CKMANAGERS_BUILD_SDLINPUT)
    add_subdirectory(SdlInputManager)
    list(APPEND CKMANAGERS_COMPONENTS SdlInputManager)
endif ()

if (CKMANAGERS_BUILD_ALL OR CKMANAGERS_BUILD_SDLSOUND)
    add_subdirectory(SdlSoundManager)
    list(APPEND CKMANAGERS_COMPONENTS SdlSoundManager)
endif ()

if (CKMANAGERS_BUILD_ALL OR CKMANAGERS_BUILD_PARAMETEROPERATIONS)
    add_subdirectory(ParameterOperations)
    list(APPEND CKMANAGERS_COMPONENTS ParameterOperations)
endif ()

if (MSVC)
    foreach (_manager IN ITEMS
            SdlInputManager SdlInputManagerStatic
            SdlSoundManager SdlSoundManagerStatic
            ParameterOperations ParameterOperationsStatic)
        if (TARGET ${_manager})
            set_target_properties(${_manager} PROPERTIES FOLDER "Managers")
        endif ()
    endforeach ()
endif ()

if (CKMANAGERS_IS_TOP_LEVEL)
    message(STATUS "")
    message(STATUS "============================================================")
    message(STATUS "CKManagers Configuration Summary")
    message(STATUS "============================================================")
    message(STATUS "  Version:              ${PROJECT_VERSION}")
    message(STATUS "  Components:           ${CKMANAGERS_COMPONENTS}")
    message(STATUS "  Install Prefix:       ${CMAKE_INSTALL_PREFIX}")
    message(STATUS "============================================================")
    message(STATUS "")
endif ()
