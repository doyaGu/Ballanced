set(GAME_BUILDING_BLOCKS
        3DTrans
        BuildingBlocksAddons1
        Cameras
        Collision
        Controllers
        Interface
        Lights
        Logics
        Materials-Textures
        Narratives
        physics_RT
        Sounds
        TT_DatabaseManager_RT
        TT_Gravity_RT
        TT_InterfaceManager_RT
        TT_ParticleSystems_RT
        TT_Toolbox_RT
        Visuals
        WorldEnvironment
        )

foreach (BB IN LISTS GAME_BUILDING_BLOCKS)
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${BB}/CMakeLists.txt")
        add_subdirectory(${BB})
    endif ()
endforeach ()