set(GAME_BUILDING_BLOCKS
        physics_RT
        TT_DatabaseManager_RT
        TT_Gravity_RT
        TT_InterfaceManager_RT
        TT_ParticleSystems_RT
        TT_Toolbox_RT
        )

foreach (BB IN LISTS GAME_BUILDING_BLOCKS)
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${BB}/CMakeLists.txt")
        add_subdirectory(${BB})
    endif ()
endforeach ()