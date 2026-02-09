# Source - Component Aggregator
# Adds all Ballanced components and configures output directories
# to match the Ballance runtime layout.

# --- Helpers ---

# Set Visual Studio folder for target
function(_set_vs_folder target folder_name)
    if (TARGET ${target})
        set_target_properties(${target} PROPERTIES FOLDER "${folder_name}")
    endif ()
endfunction()

# Set output directories for a component group
function(_set_output_dirs subdir)
    set(_root "${BALLANCED_BUILD_ROOT}/${subdir}")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${_root}" PARENT_SCOPE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${_root}" PARENT_SCOPE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib" PARENT_SCOPE)

    # Per-config output dirs for multi-config generators
    foreach (_cfg IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${_cfg} "${_root}" PARENT_SCOPE)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${_cfg} "${_root}" PARENT_SCOPE)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${_cfg} "${CMAKE_BINARY_DIR}/lib" PARENT_SCOPE)
    endforeach ()
endfunction()

# Force output directories on specific targets
function(_fix_target_output_dir target subdir)
    if (NOT TARGET ${target})
        return()
    endif ()

    set(_root "${BALLANCED_BUILD_ROOT}/${subdir}")

    set_target_properties(${target} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${_root}"
            LIBRARY_OUTPUT_DIRECTORY "${_root}"
    )

    foreach (_cfg IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
        set_target_properties(${target} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY_${_cfg} "${_root}"
                LIBRARY_OUTPUT_DIRECTORY_${_cfg} "${_root}"
        )
    endforeach ()
endfunction()

# --- Components ---

# Core (VxMath, CK2) -> Bin/
message(STATUS "[Ballanced] Adding core components...")
_set_output_dirs("Bin")
add_subdirectory(VxMath)
add_subdirectory(CK2)
_fix_target_output_dir(VxMath "Bin")
_fix_target_output_dir(VxMathStatic "Bin")
_fix_target_output_dir(CK2 "Bin")
_fix_target_output_dir(CK2Static "Bin")
if (MSVC)
    _set_vs_folder(VxMath "Core")
    _set_vs_folder(VxMathStatic "Core")
    _set_vs_folder(CK2 "Core")
    _set_vs_folder(CK2Static "Core")
endif ()

# Render Engine -> RenderEngines/
message(STATUS "[Ballanced] Adding render engine...")
_set_output_dirs("RenderEngines")
add_subdirectory(RenderEngine)
_fix_target_output_dir(CK2_3D "RenderEngines")
_fix_target_output_dir(CK2_3DStatic "RenderEngines")
_fix_target_output_dir(CKDX9Rasterizer "RenderEngines")
_fix_target_output_dir(CKGLRasterizer "RenderEngines")
if (MSVC)
    _set_vs_folder(CK2_3D "Core")
    _set_vs_folder(CK2_3DStatic "Core")
    _set_vs_folder(CKDX9Rasterizer "RenderEngines")
    _set_vs_folder(CKGLRasterizer "RenderEngines")
endif ()

# Managers -> Managers/
message(STATUS "[Ballanced] Adding managers...")
_set_output_dirs("Managers")
foreach (_mgr IN ITEMS Dx8InputManager Dx8SoundManager ParameterOperations)
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Managers/${_mgr}/CMakeLists.txt")
        add_subdirectory(Managers/${_mgr})
        if (MSVC)
            _set_vs_folder(${_mgr} "Managers")
        endif ()
    endif ()
endforeach ()

# Plugins -> Plugins/
message(STATUS "[Ballanced] Adding plugins...")
_set_output_dirs("Plugins")
add_subdirectory(Plugins)
foreach (_plugin IN ITEMS AVIReader ImageReader WavReader VirtoolsLoader)
    _fix_target_output_dir(${_plugin} "Plugins")
    if (MSVC)
        _set_vs_folder(${_plugin} "Plugins")
    endif ()
endforeach ()

# Building Blocks -> BuildingBlocks/
message(STATUS "[Ballanced] Adding building blocks...")
_set_output_dirs("BuildingBlocks")
add_subdirectory(BuildingBlocks)
if (MSVC)
    # Organize building block modules
    foreach (_bb_module IN ITEMS
            3DTrans BuildingBlocksAddons1 Cameras Characters Collision Controllers
            Grids Interface Lights Logics Materials-Textures MeshModifiers
            MidiManager Narratives Sounds Visuals WorldEnvironment
            physics_RT TT_DatabaseManager_RT TT_Gravity_RT TT_InterfaceManager_RT
            TT_ParticleSystems_RT TT_Toolbox_RT)
        _set_vs_folder(${_bb_module} "BuildingBlocks")
        _set_vs_folder(${_bb_module}Static "BuildingBlocks")
    endforeach ()

    # Organize IVP physics library targets
    _set_vs_folder(ivp_utility "BuildingBlocks/IVP")
    _set_vs_folder(ivp_surface_manager "BuildingBlocks/IVP")
    _set_vs_folder(ivp_collision "BuildingBlocks/IVP")
    _set_vs_folder(ivp_intern "BuildingBlocks/IVP")
    _set_vs_folder(ivp_physics "BuildingBlocks/IVP")
    _set_vs_folder(ivp_controller "BuildingBlocks/IVP")
    _set_vs_folder(ivp_compact_builder "BuildingBlocks/IVP")
endif ()

# Player -> Bin/
message(STATUS "[Ballanced] Adding player...")
_set_output_dirs("Bin")
add_subdirectory(Player)
if (MSVC)
    _set_vs_folder(Player "Applications")
    _set_vs_folder(ConfigTool "Applications")
endif ()

# Summary
message(STATUS "[Ballanced] Build outputs: ${BALLANCED_BUILD_ROOT}/")
message(STATUS "[Ballanced] Install outputs: ${CMAKE_INSTALL_PREFIX}/")
