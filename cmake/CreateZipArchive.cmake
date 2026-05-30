# Creates a compressed zip archive from the current working directory.
# Usage:
#   cmake -DARCHIVE_PATH=<path> -P CreateZipArchive.cmake

if (NOT DEFINED ARCHIVE_PATH OR ARCHIVE_PATH STREQUAL "")
    message(FATAL_ERROR "ARCHIVE_PATH is required")
endif ()

file(ARCHIVE_CREATE
        OUTPUT "${ARCHIVE_PATH}"
        PATHS .
        FORMAT zip
)
