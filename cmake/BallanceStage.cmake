# Root-only staging, asset installation, and layout test registration.

include(CTest)

get_property(_ballance_stage_is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if (_ballance_stage_is_multi_config)
    set(_ballance_install_cmd "${CMAKE_COMMAND}" --install "${CMAKE_BINARY_DIR}" --config "$<CONFIG>")
else ()
    set(_ballance_install_cmd "${CMAKE_COMMAND}" --install "${CMAKE_BINARY_DIR}")
endif ()

add_custom_target(stage
        COMMAND ${_ballance_install_cmd}
        COMMENT "Installing to ${CMAKE_INSTALL_PREFIX}"
        USES_TERMINAL
        VERBATIM
)

set(_ballance_runtime_targets
        VxMath CK2 CK2_3D CKDX9Rasterizer CKBgfxRasterizer
        Dx8InputManager Dx8SoundManager ParameterOperations
        AVIReader ImageReader WavReader VirtoolsLoader
        Player ConfigTool

        # Building blocks (DLL output names may differ from target names)
        3DTrans Cameras Collision Controllers Grids Interface Lights Logics Materials
        MeshModifiers MidiManager Narratives Sounds Visuals WorldEnvironment Characters
        BuildingBlocksAddons1
        physics_RT TT_DatabaseManager_RT TT_Gravity_RT TT_InterfaceManager_RT
        TT_ParticleSystems_RT TT_Toolbox_RT
)

foreach (_target IN LISTS _ballance_runtime_targets)
    if (TARGET ${_target})
        add_dependencies(stage ${_target})
    endif ()
endforeach ()

if (BALLANCE_BUILD_STATIC_PLAYER)
    set(_ballance_static_player_configs
            Source/RenderEngine/src/CK2_3D.ini
            Source/RenderEngine/src/CKRasterizer/CKBgfxRasterizer/CKBgfxRasterizer.ini
    )
    foreach (_config IN LISTS _ballance_static_player_configs)
        if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_config}")
            install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${_config}" DESTINATION Bin)
        endif ()
    endforeach ()
endif ()

set(_ballance_check_render_configs OFF)
foreach (_config IN ITEMS
        Source/RenderEngine/src/CK2_3D.ini
        Source/RenderEngine/src/CKRasterizer/CKBgfxRasterizer/CKBgfxRasterizer.ini)
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_config}")
        set(_ballance_check_render_configs ON)
    endif ()
endforeach ()

if (BALLANCE_ASSETS_ROOT AND EXISTS "${BALLANCE_ASSETS_ROOT}")
    message(STATUS "[Ballance] Staging assets from: ${BALLANCE_ASSETS_ROOT}")

    install(DIRECTORY
            "${BALLANCE_ASSETS_ROOT}/Textures"
            "${BALLANCE_ASSETS_ROOT}/Sounds"
            "${BALLANCE_ASSETS_ROOT}/Text"
            "${BALLANCE_ASSETS_ROOT}/3D Entities"
            DESTINATION . OPTIONAL
    )

    install(FILES
            "${BALLANCE_ASSETS_ROOT}/base.cmo"
            "${BALLANCE_ASSETS_ROOT}/Database.tdb"
            DESTINATION . OPTIONAL
    )
elseif (BALLANCE_ASSETS_ROOT)
    message(WARNING "[Ballance] Asset path not found: ${BALLANCE_ASSETS_ROOT}")
endif ()

if (BALLANCE_DIR AND EXISTS "${BALLANCE_DIR}")
    message(STATUS "[Ballance] Deploy target: ${BALLANCE_DIR}")
    message(STATUS "  Sync ${CMAKE_INSTALL_PREFIX} manually after staging")
elseif (BALLANCE_DIR)
    message(NOTICE "[Ballance] Deploy path not found: ${BALLANCE_DIR}")
endif ()

add_test(NAME StageInstall
        COMMAND "${CMAKE_COMMAND}"
        -DBUILD_DIR:PATH=${CMAKE_BINARY_DIR}
        -DCONFIG:STRING=${BALLANCE_TEST_CONFIG}
        -P "${CMAKE_CURRENT_LIST_DIR}/RunStageInstall.cmake"
)

add_test(NAME StageLayout
        COMMAND "${CMAKE_COMMAND}"
        -DSTAGE_ROOT:PATH=${CMAKE_INSTALL_PREFIX}
        -DSTATIC_PLAYER:BOOL=${BALLANCE_BUILD_STATIC_PLAYER}
        -DCHECK_ASSETS:BOOL=$<BOOL:${BALLANCE_ASSETS_ROOT}>
        -DCHECK_RENDER_CONFIGS:BOOL=${_ballance_check_render_configs}
        -P "${CMAKE_CURRENT_LIST_DIR}/VerifyStage.cmake"
)

set_tests_properties(StageLayout PROPERTIES DEPENDS StageInstall)

if (_ballance_stage_is_multi_config)
    set_tests_properties(StageInstall StageLayout PROPERTIES
            CONFIGURATIONS "${BALLANCE_TEST_CONFIG}"
    )
endif ()
