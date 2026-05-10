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
    if(EXISTS "${STAGE_ROOT}/${rel_without_ext}.dll")
        return()
    endif()

    # MinGW names DLL outputs with a "lib" prefix by default.
    get_filename_component(_dll_dir "${rel_without_ext}" DIRECTORY)
    get_filename_component(_dll_name "${rel_without_ext}" NAME)
    if(_dll_dir STREQUAL "")
        set(_alt_rel "lib${_dll_name}.dll")
    else()
        set(_alt_rel "${_dll_dir}/lib${_dll_name}.dll")
    endif()

    if(EXISTS "${STAGE_ROOT}/${_alt_rel}")
        return()
    endif()

    message(FATAL_ERROR "Missing file: ${STAGE_ROOT}/${rel_without_ext}.dll (or ${STAGE_ROOT}/${_alt_rel})")
endfunction()

message(STATUS "[StageLayout] Verifying: ${STAGE_ROOT}")
message(STATUS "[StageLayout] Check assets: ${CHECK_ASSETS}")
message(STATUS "[StageLayout] Check render configs: ${CHECK_RENDER_CONFIGS}")
message(STATUS "[StageLayout] Static player: ${STATIC_PLAYER}")

# Required directories
_require_dir(Bin)
_forbid_path(include)
_forbid_path(lib)

_require_file(Bin/Player.exe)

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
foreach(_mgr IN ITEMS Dx8InputManager Dx8SoundManager ParameterOperations)
    _require_dll("Managers/${_mgr}")
endforeach()

# Render engine
_require_dll("RenderEngines/CK2_3D")
if(CHECK_RENDER_CONFIGS)
    _require_file(RenderEngines/CK2_3D.ini)
endif()

set(_has_rasterizer OFF)
foreach(_rast IN ITEMS CKBgfxRasterizer CKDX9Rasterizer CKGLRasterizer)
    if(EXISTS "${STAGE_ROOT}/RenderEngines/${_rast}.dll" OR EXISTS "${STAGE_ROOT}/RenderEngines/lib${_rast}.dll")
        set(_has_rasterizer ON)
        if(CHECK_RENDER_CONFIGS AND _rast STREQUAL CKBgfxRasterizer)
            _require_file(RenderEngines/CKBgfxRasterizer.ini)
        endif()
    endif()
endforeach()
if(NOT _has_rasterizer)
    message(FATAL_ERROR "Missing rasterizer (CKBgfxRasterizer.dll/libCKBgfxRasterizer.dll, CKDX9Rasterizer.dll/libCKDX9Rasterizer.dll, or CKGLRasterizer.dll/libCKGLRasterizer.dll)")
endif()

# Plugins
foreach(_plugin IN ITEMS AVIReader ImageReader WavReader VirtoolsLoader)
    _require_dll("Plugins/${_plugin}")
endforeach()

# Sample building blocks
foreach(_bb IN ITEMS 3DTransfo Cameras Collisions)
    _require_dll("BuildingBlocks/${_bb}")
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
