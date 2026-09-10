# Verifies a release archive by extracting it and checking the staged runtime layout.
# Usage:
#   cmake -DARCHIVE_PATH=<archive> -DEXTRACT_ROOT=<dir>
#         -DBALLANCE_COMPONENT_MANIFEST=<build>/RuntimeComponents.cmake
#         [-DBALLANCE_BUILD_STATIC=ON] -P VerifyReleaseArchive.cmake

if (NOT DEFINED ARCHIVE_PATH OR ARCHIVE_PATH STREQUAL "")
    message(FATAL_ERROR "ARCHIVE_PATH is required")
endif ()

if (NOT EXISTS "${ARCHIVE_PATH}")
    message(FATAL_ERROR "Archive not found: ${ARCHIVE_PATH}")
endif ()

if (NOT DEFINED BALLANCE_COMPONENT_MANIFEST OR
        NOT EXISTS "${BALLANCE_COMPONENT_MANIFEST}")
    message(FATAL_ERROR
            "BALLANCE_COMPONENT_MANIFEST must name the configured RuntimeComponents.cmake")
endif ()

if (NOT DEFINED EXTRACT_ROOT OR EXTRACT_ROOT STREQUAL "")
    set(EXTRACT_ROOT "${CMAKE_CURRENT_BINARY_DIR}/release-archive-check")
endif ()

if (NOT DEFINED BALLANCE_BUILD_STATIC)
    set(BALLANCE_BUILD_STATIC OFF)
endif ()

message(STATUS "[ReleaseArchive] Archive: ${ARCHIVE_PATH}")
message(STATUS "[ReleaseArchive] Extract root: ${EXTRACT_ROOT}")

file(REMOVE_RECURSE "${EXTRACT_ROOT}")
file(MAKE_DIRECTORY "${EXTRACT_ROOT}")

execute_process(
        COMMAND "${CMAKE_COMMAND}" -E tar xf "${ARCHIVE_PATH}"
        WORKING_DIRECTORY "${EXTRACT_ROOT}"
        RESULT_VARIABLE _extract_result
)
if (NOT _extract_result EQUAL 0)
    message(FATAL_ERROR "Failed to extract archive: ${ARCHIVE_PATH}")
endif ()

set(STAGE_ROOT "${EXTRACT_ROOT}")
set(CHECK_ASSETS OFF)
include("${CMAKE_CURRENT_LIST_DIR}/VerifyStage.cmake")

foreach (_asset IN ITEMS
        Textures
        Sounds
        Text
        "3D Entities"
        base.cmo
        Database.tdb)
    if (EXISTS "${EXTRACT_ROOT}/${_asset}")
        message(FATAL_ERROR "Release archive must not contain game assets: ${_asset}")
    endif ()
endforeach ()

message(STATUS "[ReleaseArchive] Verification complete")
