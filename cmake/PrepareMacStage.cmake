# Removes staged Mach-O files before reinstalling them. CMake otherwise treats
# the files as up to date but repeats its install_name_tool RPATH edits, which
# produces noisy errors on every incremental stage install.

if (NOT DEFINED STAGE_ROOT OR STAGE_ROOT STREQUAL "")
    message(FATAL_ERROR "STAGE_ROOT is required")
endif ()

cmake_path(ABSOLUTE_PATH STAGE_ROOT NORMALIZE OUTPUT_VARIABLE _stage_root)
if (_stage_root STREQUAL "/")
    message(FATAL_ERROR "Refusing to prepare the filesystem root as a stage")
endif ()

set(_runtime_directories
        Bin
        RenderEngines
        Managers
        Plugins
        BuildingBlocks
)

set(_staged_mach_o_files "${_stage_root}/Bin/Player")
foreach (_directory IN LISTS _runtime_directories)
    file(GLOB _directory_libraries LIST_DIRECTORIES FALSE
            "${_stage_root}/${_directory}/*.dylib"
    )
    list(APPEND _staged_mach_o_files ${_directory_libraries})
endforeach ()

if (_staged_mach_o_files)
    file(REMOVE ${_staged_mach_o_files})
endif ()
