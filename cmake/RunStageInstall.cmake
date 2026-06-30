# Runs the 'stage' target for the active configuration
# Usage: cmake [-DSOURCE_DIR=<path>] -DBUILD_DIR=<path> [-DCONFIG=<config>] -P RunStageInstall.cmake

if(NOT DEFINED BUILD_DIR OR BUILD_DIR STREQUAL "")
    message(FATAL_ERROR "BUILD_DIR is required")
endif()

if(NOT DEFINED SOURCE_DIR OR SOURCE_DIR STREQUAL "")
    set(_cache_file "${BUILD_DIR}/CMakeCache.txt")
    if(EXISTS "${_cache_file}")
        file(STRINGS "${_cache_file}" _home_dir_line REGEX "^CMAKE_HOME_DIRECTORY:INTERNAL=")
        if(_home_dir_line)
            string(REGEX REPLACE "^CMAKE_HOME_DIRECTORY:INTERNAL=" "" SOURCE_DIR "${_home_dir_line}")
        endif()
    endif()
endif()

# Determine configuration (for multi-config generators)
set(_config "")
if(DEFINED CONFIG AND NOT CONFIG STREQUAL "")
    set(_config "${CONFIG}")
elseif(DEFINED ENV{CTEST_CONFIGURATION_TYPE})
    set(_config "$ENV{CTEST_CONFIGURATION_TYPE}")
endif()

message(STATUS "[StageInstall] Build dir: ${BUILD_DIR}")
message(STATUS "[StageInstall] Config: ${_config}")

# Windows project regeneration is intentionally suppressed for normal builds.
# Refresh the generated project files here so StageInstall sees source-list
# changes made after the build directory was first configured.
if(DEFINED SOURCE_DIR AND NOT SOURCE_DIR STREQUAL "")
    if(NOT EXISTS "${SOURCE_DIR}/CMakeLists.txt")
        message(FATAL_ERROR "Stage configure source dir is invalid: ${SOURCE_DIR}")
    endif()

    message(STATUS "[StageInstall] Configure source dir: ${SOURCE_DIR}")
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -S "${SOURCE_DIR}" -B "${BUILD_DIR}"
        RESULT_VARIABLE _configure_result
    )

    if(NOT _configure_result EQUAL 0)
        message(FATAL_ERROR "Stage configure failed (exit code: ${_configure_result})")
    endif()
endif()

# Run stage target
if(_config)
    execute_process(
        COMMAND "${CMAKE_COMMAND}" --build "${BUILD_DIR}" --target stage --config "${_config}"
        RESULT_VARIABLE _result
    )
else()
    execute_process(
        COMMAND "${CMAKE_COMMAND}" --build "${BUILD_DIR}" --target stage
        RESULT_VARIABLE _result
    )
endif()

if(NOT _result EQUAL 0)
    message(FATAL_ERROR "Stage install failed (exit code: ${_result})")
endif()
