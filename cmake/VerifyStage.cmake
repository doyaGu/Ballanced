# Verifies the staged Ballance runtime layout
# Usage: cmake -DSTAGE_ROOT=<path> [-DCHECK_ASSETS=ON] -P VerifyStage.cmake

if(NOT DEFINED STAGE_ROOT OR STAGE_ROOT STREQUAL "")
    message(FATAL_ERROR "STAGE_ROOT is required")
endif()

if(NOT DEFINED CHECK_ASSETS)
    set(CHECK_ASSETS OFF)
endif()

if(NOT DEFINED CHECK_RENDER_CONFIGS)
    set(CHECK_RENDER_CONFIGS OFF)
endif()

if(NOT DEFINED STATIC_PLAYER)
    set(STATIC_PLAYER OFF)
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
            set(_alt_rel "lib${_dll_name}${_ext}")
        else()
            set(_alt_rel "${_dll_dir}/lib${_dll_name}${_ext}")
        endif()
        if(EXISTS "${STAGE_ROOT}/${_alt_rel}")
            return()
        endif()
        if(_ext STREQUAL ".so")
            file(GLOB _versioned_matches LIST_DIRECTORIES FALSE
                    "${STAGE_ROOT}/${rel_without_ext}${_ext}*"
                    "${STAGE_ROOT}/${_alt_rel}*")
            if(_versioned_matches)
                return()
            endif()
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
            set(_alt_rel "lib${_lib_name}${_ext}")
        else()
            set(_alt_rel "${_lib_dir}/lib${_lib_name}${_ext}")
        endif()
        if(EXISTS "${STAGE_ROOT}/${_alt_rel}")
            set(_found ON)
        endif()
    endforeach()
    set(${out_var} ${_found} PARENT_SCOPE)
endfunction()

message(STATUS "[StageLayout] Verifying: ${STAGE_ROOT}")
message(STATUS "[StageLayout] Check assets: ${CHECK_ASSETS}")
message(STATUS "[StageLayout] Check render configs: ${CHECK_RENDER_CONFIGS}")
message(STATUS "[StageLayout] Static player: ${STATIC_PLAYER}")
message(STATUS "[StageLayout] Check SDL3 runtime: ${CHECK_SDL3_RUNTIME}")

# Required directories
_require_dir(Bin)
_forbid_path(include)
_forbid_path(lib)

_require_exe(Bin/Player)
if(CHECK_SDL3_RUNTIME)
    _require_dll(Bin/SDL3)
endif()

if(STATIC_PLAYER)
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
set(_required_managers SdlInputManager SdlSoundManager ParameterOperations)
foreach(_mgr IN LISTS _required_managers)
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
foreach(_plugin IN ITEMS AVIReader ImageReader WavReader VirtoolsLoader)
    _require_dll("Plugins/${_plugin}")
endforeach()

# Portable building blocks
foreach(_bb IN ITEMS
        3DTransfo BuildingBlocksAddons1 Cameras Characters Collisions Controllers
        Grids Interface Lights Logics Materials MeshModifiers Narratives Sounds
        Visuals WorldEnvironment physics_RT TT_DatabaseManager_RT TT_Gravity_RT
        TT_InterfaceManager_RT TT_ParticleSystems_RT TT_Toolbox_RT)
    _require_dll("BuildingBlocks/${_bb}")
endforeach()

_shared_library_exists(_has_midi_manager "BuildingBlocks/MidiManager")
if(_has_midi_manager)
    message(STATUS "[StageLayout] MidiManager present")
endif()

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
