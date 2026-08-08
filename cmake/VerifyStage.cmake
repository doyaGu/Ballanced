# Verifies the staged Ballance runtime layout
# Usage: cmake -DSTAGE_ROOT=<path> [-DCHECK_ASSETS=ON] -P VerifyStage.cmake

include("${CMAKE_CURRENT_LIST_DIR}/BallanceComponentRegistry.cmake")

if(NOT DEFINED STAGE_ROOT OR STAGE_ROOT STREQUAL "")
    message(FATAL_ERROR "STAGE_ROOT is required")
endif()

if(NOT DEFINED CHECK_ASSETS)
    set(CHECK_ASSETS OFF)
endif()

if(NOT DEFINED CHECK_RENDER_CONFIGS)
    set(CHECK_RENDER_CONFIGS OFF)
endif()

if(NOT DEFINED BALLANCE_BUILD_STATIC)
    set(BALLANCE_BUILD_STATIC OFF)
endif()

if(NOT DEFINED CHECK_SDL3_RUNTIME)
    set(CHECK_SDL3_RUNTIME ON)
endif()

function(_require_dir rel)
    if(NOT IS_DIRECTORY "${STAGE_ROOT}/${rel}")
        message(FATAL_ERROR "Missing directory: ${STAGE_ROOT}/${rel}")
    endif()
endfunction()

function(_require_file rel)
    if(NOT EXISTS "${STAGE_ROOT}/${rel}")
        message(FATAL_ERROR "Missing file: ${STAGE_ROOT}/${rel}")
    endif()
endfunction()

function(_forbid_path rel)
    if(EXISTS "${STAGE_ROOT}/${rel}")
        message(FATAL_ERROR "Staged runtime must not contain development artifact path: ${STAGE_ROOT}/${rel}")
    endif()
endfunction()

function(_require_dll rel_without_ext)
    get_filename_component(_dll_dir "${rel_without_ext}" DIRECTORY)
    get_filename_component(_dll_name "${rel_without_ext}" NAME)
    foreach(_ext IN ITEMS ".dll" ".so" ".dylib")
        if(EXISTS "${STAGE_ROOT}/${rel_without_ext}${_ext}")
            return()
        endif()
        if(_dll_dir STREQUAL "")
            set(_alt_rel_without_ext "lib${_dll_name}")
        else()
            set(_alt_rel_without_ext "${_dll_dir}/lib${_dll_name}")
        endif()
        set(_alt_rel "${_alt_rel_without_ext}${_ext}")
        if(EXISTS "${STAGE_ROOT}/${_alt_rel}")
            return()
        endif()
        if(_ext STREQUAL ".so")
            file(GLOB _versioned_matches LIST_DIRECTORIES FALSE
                    "${STAGE_ROOT}/${rel_without_ext}${_ext}*"
                    "${STAGE_ROOT}/${_alt_rel}*")
            foreach (_match IN LISTS _versioned_matches)
                if (EXISTS "${_match}")
                    return()
                endif ()
            endforeach ()
        elseif(_ext STREQUAL ".dylib")
            file(GLOB _versioned_matches LIST_DIRECTORIES FALSE
                    "${STAGE_ROOT}/${rel_without_ext}.*${_ext}"
                    "${STAGE_ROOT}/${_alt_rel_without_ext}.*${_ext}")
            foreach (_match IN LISTS _versioned_matches)
                if (EXISTS "${_match}")
                    return()
                endif ()
            endforeach ()
        endif()
    endforeach()

    message(FATAL_ERROR "Missing shared library for: ${STAGE_ROOT}/${rel_without_ext}")
endfunction()

function(_require_exe rel_without_ext)
    if(EXISTS "${STAGE_ROOT}/${rel_without_ext}" OR EXISTS "${STAGE_ROOT}/${rel_without_ext}.exe")
        return()
    endif()
    message(FATAL_ERROR "Missing executable: ${STAGE_ROOT}/${rel_without_ext}[.exe]")
endfunction()

function(_shared_library_exists out_var rel_without_ext)
    get_filename_component(_lib_dir "${rel_without_ext}" DIRECTORY)
    get_filename_component(_lib_name "${rel_without_ext}" NAME)
    set(_found OFF)
    foreach(_ext IN ITEMS ".dll" ".so" ".dylib")
        if(EXISTS "${STAGE_ROOT}/${rel_without_ext}${_ext}")
            set(_found ON)
        endif()
        if(_lib_dir STREQUAL "")
            set(_alt_rel_without_ext "lib${_lib_name}")
        else()
            set(_alt_rel_without_ext "${_lib_dir}/lib${_lib_name}")
        endif()
        set(_alt_rel "${_alt_rel_without_ext}${_ext}")
        if(EXISTS "${STAGE_ROOT}/${_alt_rel}")
            set(_found ON)
        endif()
        if(_ext STREQUAL ".so")
            file(GLOB _versioned_matches LIST_DIRECTORIES FALSE
                    "${STAGE_ROOT}/${rel_without_ext}${_ext}*"
                    "${STAGE_ROOT}/${_alt_rel}*")
            if(_versioned_matches)
                set(_found ON)
            endif()
        elseif(_ext STREQUAL ".dylib")
            file(GLOB _versioned_matches LIST_DIRECTORIES FALSE
                    "${STAGE_ROOT}/${rel_without_ext}.*${_ext}"
                    "${STAGE_ROOT}/${_alt_rel_without_ext}.*${_ext}")
            if(_versioned_matches)
                set(_found ON)
            endif()
        endif()
    endforeach()
    set(${out_var} ${_found} PARENT_SCOPE)
endfunction()

function(_verify_macos_sdl_runtime)
    find_program(_otool otool REQUIRED)
    file(GLOB _sdl_candidates LIST_DIRECTORIES FALSE
            "${STAGE_ROOT}/Bin/libSDL3*.dylib"
            "${STAGE_ROOT}/Bin/SDL3*.dylib")
    set(_sdl_files "")
    foreach (_candidate IN LISTS _sdl_candidates)
        if (NOT IS_SYMLINK "${_candidate}")
            list(APPEND _sdl_files "${_candidate}")
        endif ()
    endforeach ()
    list(LENGTH _sdl_files _sdl_file_count)
    if (NOT _sdl_file_count EQUAL 1)
        message(FATAL_ERROR
                "Expected one regular staged SDL3 dylib, found ${_sdl_file_count}")
    endif ()

    list(GET _sdl_files 0 _sdl_path)
    get_filename_component(_sdl_name "${_sdl_path}" NAME)
    set(_sdl_install_name "@rpath/${_sdl_name}")
    execute_process(
            COMMAND "${_otool}" -D "${_sdl_path}"
            OUTPUT_VARIABLE _sdl_id_output
            COMMAND_ERROR_IS_FATAL ANY)
    string(REPLACE "\n" ";" _sdl_id_lines "${_sdl_id_output}")
    list(FILTER _sdl_id_lines EXCLUDE REGEX "^$")
    list(GET _sdl_id_lines -1 _sdl_id)
    string(STRIP "${_sdl_id}" _sdl_id)
    if (NOT _sdl_id STREQUAL _sdl_install_name)
        message(FATAL_ERROR
                "SDL3 is not relocatable: expected install name "
                "${_sdl_install_name}, found ${_sdl_id}. "
                "Use an SDL3 package built with a relocatable install name.")
    endif ()

    set(_mach_o_files "${STAGE_ROOT}/Bin/Player")
    foreach (_directory IN ITEMS Bin RenderEngines Managers Plugins BuildingBlocks)
        file(GLOB _libraries LIST_DIRECTORIES FALSE
                "${STAGE_ROOT}/${_directory}/*.dylib")
        list(APPEND _mach_o_files ${_libraries})
    endforeach ()
    foreach (_mach_o IN LISTS _mach_o_files)
        if (IS_SYMLINK "${_mach_o}")
            continue()
        endif ()
        execute_process(
                COMMAND "${_otool}" -L "${_mach_o}"
                OUTPUT_VARIABLE _dependencies
                COMMAND_ERROR_IS_FATAL ANY)
        string(REGEX MATCHALL "[\t ]+[^\t \n]+" _dependency_fields
                "${_dependencies}")
        foreach (_field IN LISTS _dependency_fields)
            string(STRIP "${_field}" _dependency)
            get_filename_component(_dependency_name "${_dependency}" NAME)
            if (_dependency_name STREQUAL _sdl_name AND
                    NOT _dependency STREQUAL _sdl_install_name)
                message(FATAL_ERROR
                        "Non-relocatable SDL3 dependency in ${_mach_o}: "
                        "${_dependency}")
            endif ()
        endforeach ()
    endforeach ()
endfunction()

message(STATUS "[StageLayout] Verifying: ${STAGE_ROOT}")
message(STATUS "[StageLayout] Check assets: ${CHECK_ASSETS}")
message(STATUS "[StageLayout] Check render configs: ${CHECK_RENDER_CONFIGS}")
message(STATUS "[StageLayout] Static build: ${BALLANCE_BUILD_STATIC}")
message(STATUS "[StageLayout] Check SDL3 runtime: ${CHECK_SDL3_RUNTIME}")

# Required directories
_require_dir(Bin)
_forbid_path(include)
_forbid_path(lib)

_require_exe(Bin/Player)
_require_file(Bin/BallancedBuildManifest.json)
file(READ "${STAGE_ROOT}/Bin/BallancedBuildManifest.json" _build_manifest)
string(JSON _manifest_schema ERROR_VARIABLE _manifest_error GET "${_build_manifest}" schemaVersion)
if (NOT _manifest_error STREQUAL "NOTFOUND" OR NOT _manifest_schema EQUAL 1)
    message(FATAL_ERROR "Invalid Bin/BallancedBuildManifest.json")
endif ()
if(CHECK_SDL3_RUNTIME)
    _require_dll(Bin/SDL3)
    if(APPLE)
        _verify_macos_sdl_runtime()
    endif()
endif()

if(BALLANCE_BUILD_STATIC)
    if(CHECK_RENDER_CONFIGS AND EXISTS "${STAGE_ROOT}/Bin/CK2_3D.ini")
        _require_file(Bin/CK2_3D.ini)
    endif()

    if(CHECK_ASSETS)
        message(STATUS "[StageLayout] Verifying game assets...")
        foreach(_dir IN ITEMS Textures Sounds Text "3D Entities")
            _require_dir(${_dir})
        endforeach()
        foreach(_file IN ITEMS base.cmo Database.tdb)
            _require_file(${_file})
        endforeach()
    endif()

    message(STATUS "[StageLayout] Verification complete")
    return()
endif()

foreach(_dir IN ITEMS Managers RenderEngines Plugins BuildingBlocks)
    _require_dir(${_dir})
endforeach()

# Core binaries
_require_dll(Bin/CK2)
_require_dll(Bin/VxMath)

# Managers
foreach(_mgr IN LISTS BALLANCE_MANAGER_RUNTIME_OUTPUTS)
    _require_dll("Managers/${_mgr}")
endforeach()

# Render engine
_require_dll("RenderEngines/CK2_3D")
if(CHECK_RENDER_CONFIGS)
    _require_file(RenderEngines/CK2_3D.ini)
endif()

_require_dll("RenderEngines/CKBgfxRasterizer")
if(CHECK_RENDER_CONFIGS)
    _require_file(RenderEngines/CKBgfxRasterizer.ini)
endif()

# Plugins
foreach(_plugin IN LISTS BALLANCE_PLUGIN_RUNTIME_OUTPUTS)
    _require_dll("Plugins/${_plugin}")
endforeach()

# Portable building blocks
foreach(_bb IN LISTS BALLANCE_BUILDING_BLOCK_REQUIRED_OUTPUTS)
    _require_dll("BuildingBlocks/${_bb}")
endforeach()

foreach(_bb IN LISTS BALLANCE_BUILDING_BLOCK_OPTIONAL_OUTPUTS)
    _shared_library_exists(_has_optional_bb "BuildingBlocks/${_bb}")
    if(_has_optional_bb)
        message(STATUS "[StageLayout] ${_bb} present")
    endif()
endforeach()

# Game assets (optional)
if(CHECK_ASSETS)
    message(STATUS "[StageLayout] Verifying game assets...")
    foreach(_dir IN ITEMS Textures Sounds Text "3D Entities")
        _require_dir(${_dir})
    endforeach()
    foreach(_file IN ITEMS base.cmo Database.tdb)
        _require_file(${_file})
    endforeach()
endif()

message(STATUS "[StageLayout] Verification complete")
