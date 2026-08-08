if (NOT BALLANCE_SDL_SOURCE OR NOT BALLANCE_SDL_NAME)
    message(FATAL_ERROR "BALLANCE_SDL_SOURCE and BALLANCE_SDL_NAME are required")
endif ()

find_program(_install_name_tool install_name_tool REQUIRED)
find_program(_otool otool REQUIRED)
find_program(_codesign codesign REQUIRED)

file(REAL_PATH "${BALLANCE_SDL_SOURCE}" _sdl_real_path)
set(_bin_dir "${CMAKE_INSTALL_PREFIX}/Bin")
set(_staged_sdl "${_bin_dir}/${BALLANCE_SDL_NAME}")
file(MAKE_DIRECTORY "${_bin_dir}")
file(COPY_FILE "${_sdl_real_path}" "${_staged_sdl}" ONLY_IF_DIFFERENT)

set(_relocatable_sdl_name "@rpath/${BALLANCE_SDL_NAME}")
execute_process(
        COMMAND "${_install_name_tool}" -id "${_relocatable_sdl_name}" "${_staged_sdl}"
        COMMAND_ERROR_IS_FATAL ANY
)

# Imported SDL packages may encode a package-manager path in every consumer.
# Rewrite only dependencies whose basename matches the staged SDL runtime.
set(_mach_o_files "${_bin_dir}/Player")
foreach (_directory IN ITEMS Bin RenderEngines Managers Plugins BuildingBlocks)
    file(GLOB _libraries LIST_DIRECTORIES FALSE
            "${CMAKE_INSTALL_PREFIX}/${_directory}/*.dylib")
    list(APPEND _mach_o_files ${_libraries})
endforeach ()

foreach (_mach_o IN LISTS _mach_o_files)
    if (NOT EXISTS "${_mach_o}" OR IS_SYMLINK "${_mach_o}")
        continue()
    endif ()

    set(_needs_codesign OFF)
    if ("${_mach_o}" STREQUAL "${_staged_sdl}")
        set(_needs_codesign ON)
    endif ()

    execute_process(
            COMMAND "${_otool}" -L "${_mach_o}"
            OUTPUT_VARIABLE _dependencies
            COMMAND_ERROR_IS_FATAL ANY
    )
    string(REGEX MATCHALL "[\t ]+[^\t \n]+" _dependency_fields "${_dependencies}")
    foreach (_field IN LISTS _dependency_fields)
        string(STRIP "${_field}" _dependency)
        get_filename_component(_dependency_name "${_dependency}" NAME)
        if ("${_dependency_name}" STREQUAL "${BALLANCE_SDL_NAME}" AND
                NOT "${_dependency}" STREQUAL "${_relocatable_sdl_name}")
            execute_process(
                    COMMAND "${_install_name_tool}" -change
                            "${_dependency}" "${_relocatable_sdl_name}" "${_mach_o}"
                    COMMAND_ERROR_IS_FATAL ANY
            )
            set(_needs_codesign ON)
        endif ()
    endforeach ()

    if (_needs_codesign)
        execute_process(
                COMMAND "${_codesign}" --force --sign - "${_mach_o}"
                COMMAND_ERROR_IS_FATAL ANY
        )
    endif ()
endforeach ()
