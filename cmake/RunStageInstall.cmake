# Runs the 'stage' target for the active configuration
# Usage: cmake -DBUILD_DIR=<path> [-DCONFIG=<config>] -P RunStageInstall.cmake

if(NOT DEFINED BUILD_DIR OR BUILD_DIR STREQUAL "")
    message(FATAL_ERROR "BUILD_DIR is required")
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
