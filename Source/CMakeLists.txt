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

# --- Components ---

# Core (VxMath, CK2) -> Bin/
message(STATUS "[Ballance] Adding core components...")
add_subdirectory(VxMath)
add_subdirectory(CK2)
if (MSVC)
    _set_vs_folder(VxMath "Core")
    _set_vs_folder(VxMathStatic "Core")
    _set_vs_folder(CK2 "Core")
    _set_vs_folder(CK2Static "Core")
endif ()

# Render Engine -> RenderEngines/
message(STATUS "[Ballance] Adding render engine...")
add_subdirectory(RenderEngine)
if (MSVC)
    _set_vs_folder(CK2_3D "Core")
    _set_vs_folder(CK2_3DStatic "Core")
    _set_vs_folder(CKDX9Rasterizer "RenderEngines")
    _set_vs_folder(CKGLRasterizer "RenderEngines")
endif ()

# Managers -> Managers/
message(STATUS "[Ballance] Adding managers...")
foreach (_mgr IN ITEMS Dx8InputManager Dx8SoundManager ParameterOperations)
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Managers/${_mgr}/CMakeLists.txt")
        add_subdirectory(Managers/${_mgr})
        if (MSVC)
            _set_vs_folder(${_mgr} "Managers")
        endif ()
    endif ()
endforeach ()

# Plugins -> Plugins/
message(STATUS "[Ballance] Adding plugins...")
add_subdirectory(Plugins)
foreach (_plugin IN ITEMS AVIReader ImageReader WavReader VirtoolsLoader)
    if (MSVC)
        _set_vs_folder(${_plugin} "Plugins")
    endif ()
endforeach ()

# Building Blocks -> BuildingBlocks/
message(STATUS "[Ballance] Adding building blocks...")
add_subdirectory(BuildingBlocks)
if (MSVC)
    # Organize building block modules
    foreach (_bb_module IN ITEMS
            3DTrans BuildingBlocksAddons1 Cameras Characters Collision Controllers
            Grids Interface Lights Logics Materials MeshModifiers
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
message(STATUS "[Ballance] Adding player...")
add_subdirectory(Player)
if (MSVC)
    _set_vs_folder(Player "Applications")
    _set_vs_folder(ConfigTool "Applications")
endif ()

# Summary
message(STATUS "[Ballance] Install outputs: ${CMAKE_INSTALL_PREFIX}/")
