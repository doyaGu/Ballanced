# Packages a staged Ballance runtime layout.
# Usage:
#   cmake -DSTAGE_ROOT=<path> -DARCHIVE_PATH=<path> -P PackageStage.cmake

if (NOT DEFINED STAGE_ROOT OR STAGE_ROOT STREQUAL "")
    message(FATAL_ERROR "STAGE_ROOT is required")
endif ()

if (NOT IS_DIRECTORY "${STAGE_ROOT}")
    message(FATAL_ERROR "Stage root not found: ${STAGE_ROOT}")
endif ()

if (NOT DEFINED ARCHIVE_PATH OR ARCHIVE_PATH STREQUAL "")
    message(FATAL_ERROR "ARCHIVE_PATH is required")
endif ()

get_filename_component(_archive_dir "${ARCHIVE_PATH}" DIRECTORY)
if (NOT _archive_dir STREQUAL "")
    file(MAKE_DIRECTORY "${_archive_dir}")
endif ()

file(REMOVE "${ARCHIVE_PATH}")

if (ARCHIVE_PATH MATCHES "\\.tar\\.gz$" OR ARCHIVE_PATH MATCHES "\\.tgz$")
    set(_tar_mode "cfz")
elseif (ARCHIVE_PATH MATCHES "\\.tar\\.bz2$" OR ARCHIVE_PATH MATCHES "\\.tbz2$")
    set(_tar_mode "cfj")
elseif (ARCHIVE_PATH MATCHES "\\.tar\\.xz$" OR ARCHIVE_PATH MATCHES "\\.txz$")
    set(_tar_mode "cfJ")
elseif (ARCHIVE_PATH MATCHES "\\.zip$")
    set(_zip_archive TRUE)
else ()
    message(FATAL_ERROR "Unsupported archive extension: ${ARCHIVE_PATH}")
endif ()

message(STATUS "[PackageStage] Stage root: ${STAGE_ROOT}")
message(STATUS "[PackageStage] Archive: ${ARCHIVE_PATH}")

if (_zip_archive)
    execute_process(
            COMMAND "${CMAKE_COMMAND}" -E chdir "${STAGE_ROOT}"
                    "${CMAKE_COMMAND}" "-DARCHIVE_PATH:PATH=${ARCHIVE_PATH}" -P "${CMAKE_CURRENT_LIST_DIR}/CreateZipArchive.cmake"
            RESULT_VARIABLE _package_result
    )
else ()
    execute_process(
            COMMAND "${CMAKE_COMMAND}" -E tar "${_tar_mode}" "${ARCHIVE_PATH}" .
            WORKING_DIRECTORY "${STAGE_ROOT}"
            RESULT_VARIABLE _package_result
    )
endif ()
if (NOT _package_result EQUAL 0)
    message(FATAL_ERROR "Failed to create archive: ${ARCHIVE_PATH}")
endif ()

message(STATUS "[PackageStage] Package complete")
