# Verifies the staged Ballance runtime layout
# Usage: cmake -DSTAGE_ROOT=<path> [-DCHECK_ASSETS=ON] -P VerifyStage.cmake

if(NOT DEFINED STAGE_ROOT OR STAGE_ROOT STREQUAL "")
    message(FATAL_ERROR "STAGE_ROOT is required")
endif()

if(NOT DEFINED CHECK_ASSETS)
    set(CHECK_ASSETS OFF)
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

message(STATUS "[StageLayout] Verifying: ${STAGE_ROOT}")
message(STATUS "[StageLayout] Check assets: ${CHECK_ASSETS}")

# Required directories
foreach(_dir IN ITEMS Bin Managers RenderEngines Plugins BuildingBlocks)
    _require_dir(${_dir})
endforeach()

# Core binaries
foreach(_bin IN ITEMS Bin/Player.exe Bin/CK2.dll Bin/VxMath.dll)
    _require_file(${_bin})
endforeach()

# Managers
foreach(_mgr IN ITEMS Dx8InputManager Dx8SoundManager ParameterOperations)
    _require_file("Managers/${_mgr}.dll")
endforeach()

# Render engine
_require_file("RenderEngines/CK2_3D.dll")

set(_has_rasterizer OFF)
foreach(_rast IN ITEMS CKDX9Rasterizer CKGLRasterizer)
    if(EXISTS "${STAGE_ROOT}/RenderEngines/${_rast}.dll")
        set(_has_rasterizer ON)
    endif()
endforeach()
if(NOT _has_rasterizer)
    message(FATAL_ERROR "Missing rasterizer (CKDX9Rasterizer.dll or CKGLRasterizer.dll)")
endif()

# Plugins
foreach(_plugin IN ITEMS AVIReader ImageReader WavReader VirtoolsLoader)
    _require_file("Plugins/${_plugin}.dll")
endforeach()

# Sample building blocks
foreach(_bb IN ITEMS 3DTransfo Cameras Collisions)
    _require_file("BuildingBlocks/${_bb}.dll")
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
