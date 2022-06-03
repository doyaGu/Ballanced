cmake_minimum_required(VERSION 3.12)

project(Ballanced
    VERSION 0.1.5
    DESCRIPTION "Ballance Decompilation"
    HOMEPAGE_URL https://github.com/doyaGu/Ballanced
    LANGUAGES C CXX
)

set(CMAKE_C_STANDARD 90)
set(CMAKE_CXX_STANDARD 98)

# Set CMake modules search path
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(BuildingBlockUtils)

# Use folders to organize targets in an IDE
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")

if(NOT WIN32)
	message(FATAL_ERROR "Only Windows is supported.")
endif()

if(MSVC)
    string(TOLOWER ${CMAKE_GENERATOR_PLATFORM} TARGET_PLATFORM)
    if(NOT ("${TARGET_PLATFORM}" STREQUAL "win32"))
        message(FATAL_ERROR "You can only build 32-bit executable since only 32-bit SDK is provided.")
    endif()
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as no build type was specified")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the build type (Debug/Release)" FORCE)
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}" CACHE PATH "The root directory of the installation" FORCE)
    message(STATUS "Setting defalut install directory to ${CMAKE_INSTALL_PREFIX} as no install directory was specified")
endif()

if(NOT BUILDING_BLOCKS_DIR)
    set(BUILDING_BLOCKS_DIR "BuildingBlocks" CACHE PATH "The directory of the building blocks" FORCE)
    message(STATUS "Setting defalut building blocks directory to ${CMAKE_INSTALL_PREFIX}/${BUILDING_BLOCKS_DIR}")
endif()

set(VIRTOOLS_SDK_PATH "${VIRTOOLS_SDK_PATH}" CACHE PATH "Path to the Virtools SDK")
set(VIRTOOLS_SDK_FETCH_FROM_GIT "${VIRTOOLS_SDK_FETCH_FROM_GIT}" CACHE BOOL "Set to ON to fetch copy of SDK from git if not otherwise locatable")
set(VIRTOOLS_SDK_FETCH_FROM_GIT_PATH "${VIRTOOLS_SDK_FETCH_FROM_GIT_PATH}" CACHE FILEPATH "location to download SDK")

if(NOT VIRTOOLS_SDK_PATH)
    if (NOT VIRTOOLS_SDK_FETCH_FROM_GIT)
        message(FATAL_ERROR "Virtools SDK location was not specified. Please set VIRTOOLS_SDK_PATH or set VIRTOOLS_SDK_FETCH_FROM_GIT to on to fetch from git.")
    else()
        include(FetchContent)
        set(FETCHCONTENT_BASE_DIR_SAVE ${FETCHCONTENT_BASE_DIR})
        if (VIRTOOLS_SDK_FETCH_FROM_GIT_PATH)
            get_filename_component(FETCHCONTENT_BASE_DIR "${VIRTOOLS_SDK_FETCH_FROM_GIT_PATH}" REALPATH BASE_DIR "${CMAKE_SOURCE_DIR}")
        endif()
        FetchContent_Declare(
            Virtools_SDK
            GIT_REPOSITORY https://github.com/doyaGu/Virtools-SDK-2.1.git
            GIT_TAG main
        )
        if (NOT Virtools_SDK)
            message("Downloading Virtools SDK")
            FetchContent_Populate(Virtools_SDK)
            set(VIRTOOLS_SDK_PATH "${virtools_sdk_SOURCE_DIR}" CACHE PATH "Path to the Virtools SDK" FORCE)
        endif()
        set(FETCHCONTENT_BASE_DIR ${FETCHCONTENT_BASE_DIR_SAVE})
    endif()
endif()

find_package(VirtoolsSDK REQUIRED)

# Disable msvc unsafe warnings
add_compile_definitions(
    $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
    $<$<C_COMPILER_ID:MSVC>:_CRT_NONSTDC_NO_WARNINGS>
)

if(MINGW)
    add_compile_options($<$<C_COMPILER_ID:GNU>:-m32>)
    add_link_options($<$<C_COMPILER_ID:GNU>:-m32>)
endif()

add_subdirectory(Source)

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "Kakuty")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CMAKE_PROJECT_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}")
set(CPACK_GENERATOR "ZIP")
include(CPack)