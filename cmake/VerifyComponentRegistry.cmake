cmake_minimum_required(VERSION 3.21)

if (NOT SOURCE_DIR)
    message(FATAL_ERROR "SOURCE_DIR is required")
endif ()

include("${SOURCE_DIR}/cmake/BallanceComponentRegistry.cmake")
message(STATUS "[ComponentRegistry] Player requirements are available in the configured engine")
