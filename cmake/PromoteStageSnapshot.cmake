if (NOT DEFINED BUILD_ROOT OR NOT DEFINED SNAPSHOT_ROOT OR NOT DEFINED STAGE_ROOT)
    message(FATAL_ERROR "BUILD_ROOT, SNAPSHOT_ROOT, and STAGE_ROOT are required")
endif ()

cmake_path(ABSOLUTE_PATH BUILD_ROOT NORMALIZE OUTPUT_VARIABLE _build_root)
cmake_path(ABSOLUTE_PATH SNAPSHOT_ROOT NORMALIZE OUTPUT_VARIABLE _snapshot_root)
cmake_path(ABSOLUTE_PATH STAGE_ROOT NORMALIZE OUTPUT_VARIABLE _stage_root)
cmake_path(IS_PREFIX _build_root "${_snapshot_root}" NORMALIZE _snapshot_in_build)
cmake_path(IS_PREFIX _build_root "${_stage_root}" NORMALIZE _stage_in_build)

if (NOT _snapshot_in_build OR NOT _stage_in_build)
    message(FATAL_ERROR "Stage promotion is restricted to the build tree")
endif ()
if (_snapshot_root STREQUAL _build_root OR _stage_root STREQUAL _build_root OR
        _snapshot_root STREQUAL _stage_root)
    message(FATAL_ERROR "Invalid stage promotion paths")
endif ()
if (NOT IS_DIRECTORY "${_snapshot_root}")
    message(FATAL_ERROR "Stage snapshot does not exist: ${_snapshot_root}")
endif ()

file(REMOVE_RECURSE "${_stage_root}")
file(RENAME "${_snapshot_root}" "${_stage_root}")
