# Root runtime component registry.
#
# Component repositories keep their own standalone CMake declarations. This
# registry only describes how the superproject assembles and verifies them.

include_guard(GLOBAL)

set(BALLANCE_CORE_RUNTIME_TARGETS
        VxMath CK2
)

if (BALLANCE_COMPONENT_MANIFEST)
    include("${BALLANCE_COMPONENT_MANIFEST}")
else ()
    get_property(BALLANCE_RENDER_ENGINE_RUNTIME_TARGETS GLOBAL PROPERTY CKRE_RUNTIME_TARGETS)
    get_property(PLAYER_DECLARED_RUNTIME_TARGETS GLOBAL PROPERTY PLAYER_RUNTIME_MODULES)
endif ()
set(BALLANCE_RENDER_ENGINE_RUNTIME_OUTPUTS ${BALLANCE_RENDER_ENGINE_RUNTIME_TARGETS})

set(BALLANCE_MANAGER_RUNTIME_TARGETS
        SdlInputManager SdlSoundManager ParameterOperations
)
set(BALLANCE_MANAGER_RUNTIME_OUTPUTS
        SdlInputManager SdlSoundManager ParameterOperations
)

set(BALLANCE_PLUGIN_RUNTIME_TARGETS
        AVIReader ImageReader WavReader VirtoolsLoader
)
set(BALLANCE_PLUGIN_RUNTIME_OUTPUTS
        AVIReader ImageReader WavReader VirtoolsLoader
)

set(BALLANCE_BUILDING_BLOCK_RUNTIME_TARGETS
        3DTrans BuildingBlocksAddons1 Cameras Characters Collision Controllers
        Grids Interface Lights Logics Materials MeshModifiers MidiManager Narratives
        Sounds Visuals WorldEnvironment physics_RT TT_DatabaseManager_RT
        TT_Gravity_RT TT_InterfaceManager_RT TT_ParticleSystems_RT TT_Toolbox_RT
)
set(BALLANCE_BUILDING_BLOCK_REQUIRED_OUTPUTS
        3DTransfo BuildingBlocksAddons1 Cameras Characters Collisions Controllers
        Grids Interface Lights Logics Materials MeshModifiers Narratives Sounds
        Visuals WorldEnvironment physics_RT TT_DatabaseManager_RT TT_Gravity_RT
        TT_InterfaceManager_RT TT_ParticleSystems_RT TT_Toolbox_RT
)
set(BALLANCE_BUILDING_BLOCK_OPTIONAL_OUTPUTS
        MidiManager
)

set(BALLANCE_MODULE_RUNTIME_TARGETS
        ${BALLANCE_RENDER_ENGINE_RUNTIME_TARGETS}
        ${BALLANCE_MANAGER_RUNTIME_TARGETS}
        ${BALLANCE_PLUGIN_RUNTIME_TARGETS}
        ${BALLANCE_BUILDING_BLOCK_RUNTIME_TARGETS}
)

set(BALLANCE_RUNTIME_TARGETS
        ${BALLANCE_CORE_RUNTIME_TARGETS}
        ${BALLANCE_MODULE_RUNTIME_TARGETS}
        Player
)
