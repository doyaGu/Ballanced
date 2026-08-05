# Root-only staging, asset installation, and layout test registration.

include(CTest)

set(_ballance_can_run_target_executables ON)
if (WIN32 AND CMAKE_GENERATOR_PLATFORM)
    string(TOLOWER "${CMAKE_GENERATOR_PLATFORM}" _ballance_target_platform_lc)
    string(TOLOWER "${CMAKE_HOST_SYSTEM_PROCESSOR}" _ballance_host_processor_lc)
    if (_ballance_target_platform_lc STREQUAL "arm64" AND
            NOT _ballance_host_processor_lc MATCHES "^(arm64|aarch64)$")
        set(_ballance_can_run_target_executables OFF)
    endif ()
endif ()

set(_ballance_sdl3_runtime_target "")
if (TARGET SDL3::SDL3-shared)
    set(_ballance_sdl3_runtime_target SDL3::SDL3-shared)
elseif (TARGET SDL3::SDL3)
    get_target_property(_ballance_sdl3_runtime_target SDL3::SDL3 ALIASED_TARGET)
    if (NOT _ballance_sdl3_runtime_target)
        set(_ballance_sdl3_runtime_target SDL3::SDL3)
    endif ()
    get_target_property(_ballance_sdl3_type ${_ballance_sdl3_runtime_target} TYPE)
    get_target_property(_ballance_sdl3_imported ${_ballance_sdl3_runtime_target} IMPORTED)
    if (NOT _ballance_sdl3_imported OR NOT _ballance_sdl3_type STREQUAL "SHARED_LIBRARY")
        set(_ballance_sdl3_runtime_target "")
    endif ()
endif ()

if (BUILD_TESTING AND TARGET SDL3::SDL3 AND _ballance_can_run_target_executables)
    add_executable(PlayerSdlShortcutsTest
            "${CMAKE_SOURCE_DIR}/Source/Player/tests/PlayerSdlShortcutsTest.cpp"
            "${CMAKE_SOURCE_DIR}/Source/Player/src/PlayerSdlShortcuts.cpp"
    )
    target_compile_features(PlayerSdlShortcutsTest PRIVATE cxx_std_17)
    target_include_directories(PlayerSdlShortcutsTest PRIVATE
            "${CMAKE_SOURCE_DIR}/Source/Player/src"
    )
    target_link_libraries(PlayerSdlShortcutsTest PRIVATE SDL3::SDL3)
    set_target_properties(PlayerSdlShortcutsTest PROPERTIES FOLDER "Tests")
    if (_ballance_sdl3_runtime_target)
        set_target_properties(PlayerSdlShortcutsTest PROPERTIES
                BUILD_WITH_INSTALL_RPATH FALSE
                BUILD_RPATH "$<TARGET_FILE_DIR:${_ballance_sdl3_runtime_target}>"
        )
    endif ()
    add_test(NAME PlayerSdlShortcutsTest COMMAND PlayerSdlShortcutsTest)
elseif (BUILD_TESTING AND NOT _ballance_can_run_target_executables)
    message(STATUS "PlayerSdlShortcutsTest disabled: host cannot run target executables")
endif ()

get_property(_ballance_stage_is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if (_ballance_stage_is_multi_config)
    set(_ballance_install_cmd "${CMAKE_COMMAND}" --install "${CMAKE_BINARY_DIR}" --config "$<CONFIG>" --component Runtime)
else ()
    set(_ballance_install_cmd "${CMAKE_COMMAND}" --install "${CMAKE_BINARY_DIR}" --component Runtime)
endif ()

if (APPLE)
    set(_ballance_stage_snapshot "${CMAKE_BINARY_DIR}/stage-install-snapshot")
    add_custom_target(stage
            COMMAND "${CMAKE_COMMAND}" -E rm -rf "${_ballance_stage_snapshot}"
            COMMAND ${_ballance_install_cmd} --prefix "${_ballance_stage_snapshot}"
            COMMAND "${CMAKE_COMMAND}"
            -DBUILD_ROOT:PATH=${CMAKE_BINARY_DIR}
            -DSNAPSHOT_ROOT:PATH=${_ballance_stage_snapshot}
            -DSTAGE_ROOT:PATH=${CMAKE_INSTALL_PREFIX}
            -P "${CMAKE_CURRENT_LIST_DIR}/PromoteStageSnapshot.cmake"
            COMMENT "Installing to ${CMAKE_INSTALL_PREFIX}"
            USES_TERMINAL
            VERBATIM
    )
else ()
    add_custom_target(stage
            COMMAND ${_ballance_install_cmd}
            COMMENT "Installing to ${CMAKE_INSTALL_PREFIX}"
            USES_TERMINAL
            VERBATIM
    )
endif ()

set(_ballance_runtime_targets
        VxMath CK2 CK2_3D CKBgfxRasterizer
        SdlInputManager SdlSoundManager ParameterOperations
        AVIReader ImageReader WavReader VirtoolsLoader
        Player

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

if (TARGET PlayerSdlShortcutsTest)
    add_dependencies(stage PlayerSdlShortcutsTest)
endif ()

set(_ballance_check_sdl3_runtime OFF)
if (_ballance_sdl3_runtime_target)
    set(_ballance_check_sdl3_runtime ON)
    if (APPLE)
        set(_ballance_sdl3_imported_configs "")
        if (CMAKE_BUILD_TYPE)
            string(TOUPPER "${CMAKE_BUILD_TYPE}" _ballance_sdl3_build_config)
            get_target_property(_ballance_sdl3_mapped_configs
                    ${_ballance_sdl3_runtime_target}
                    MAP_IMPORTED_CONFIG_${_ballance_sdl3_build_config})
            if (_ballance_sdl3_mapped_configs)
                list(APPEND _ballance_sdl3_imported_configs
                        ${_ballance_sdl3_mapped_configs})
            endif ()
            list(APPEND _ballance_sdl3_imported_configs
                    "${_ballance_sdl3_build_config}")
        endif ()

        # Follow CMake's imported-target configuration selection order so the
        # staged runtime is the same library that target linking selected.
        list(APPEND _ballance_sdl3_imported_configs NOCONFIG)
        get_target_property(_ballance_sdl3_available_configs
                ${_ballance_sdl3_runtime_target} IMPORTED_CONFIGURATIONS)
        if (_ballance_sdl3_available_configs)
            list(APPEND _ballance_sdl3_imported_configs
                    ${_ballance_sdl3_available_configs})
        endif ()
        list(REMOVE_DUPLICATES _ballance_sdl3_imported_configs)

        set(_ballance_sdl3_runtime_location "")
        foreach (_ballance_sdl3_imported_config
                IN LISTS _ballance_sdl3_imported_configs)
            string(TOUPPER "${_ballance_sdl3_imported_config}"
                    _ballance_sdl3_imported_config)
            get_target_property(_ballance_sdl3_config_location
                    ${_ballance_sdl3_runtime_target}
                    IMPORTED_LOCATION_${_ballance_sdl3_imported_config})
            if (_ballance_sdl3_config_location)
                set(_ballance_sdl3_runtime_location
                        "${_ballance_sdl3_config_location}")
                break()
            endif ()
        endforeach ()

        if (NOT _ballance_sdl3_runtime_location)
            get_target_property(_ballance_sdl3_runtime_location
                    ${_ballance_sdl3_runtime_target} IMPORTED_LOCATION)
        endif ()
        if (NOT _ballance_sdl3_runtime_location)
            message(FATAL_ERROR "Unable to locate the imported SDL3 runtime")
        endif ()

        file(REAL_PATH "${_ballance_sdl3_runtime_location}"
                _ballance_sdl3_runtime_real_path)
        get_filename_component(_ballance_sdl3_runtime_name
                "${_ballance_sdl3_runtime_location}" NAME)
        install(FILES "${_ballance_sdl3_runtime_real_path}"
                DESTINATION Bin
                RENAME "${_ballance_sdl3_runtime_name}"
                COMPONENT Runtime)

        find_program(_ballance_otool otool REQUIRED)
        find_program(_ballance_codesign codesign REQUIRED)
        configure_file(
                "${CMAKE_CURRENT_LIST_DIR}/FixupMacStage.cmake.in"
                "${CMAKE_CURRENT_BINARY_DIR}/FixupMacStage.cmake"
                @ONLY)
        install(SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/FixupMacStage.cmake"
                COMPONENT Runtime)
    else ()
        install(IMPORTED_RUNTIME_ARTIFACTS ${_ballance_sdl3_runtime_target}
                RUNTIME DESTINATION Bin COMPONENT Runtime
                LIBRARY DESTINATION Bin COMPONENT Runtime
                FRAMEWORK DESTINATION Bin COMPONENT Runtime
        )
    endif ()
endif ()

if (BALLANCE_BUILD_STATIC)
    set(_ballance_static_configs
            Source/RenderEngine/src/CK2_3D.ini
            Source/RenderEngine/src/CKRasterizer/CKBgfxRasterizer/CKBgfxRasterizer.ini
    )
    foreach (_config IN LISTS _ballance_static_configs)
        if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_config}")
            install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${_config}" DESTINATION Bin COMPONENT Runtime)
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
            COMPONENT Runtime
    )

    install(FILES
            "${BALLANCE_ASSETS_ROOT}/base.cmo"
            "${BALLANCE_ASSETS_ROOT}/Database.tdb"
            DESTINATION . OPTIONAL
            COMPONENT Runtime
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
        -DSOURCE_DIR:PATH=${CMAKE_SOURCE_DIR}
        -DBUILD_DIR:PATH=${CMAKE_BINARY_DIR}
        -DCONFIG:STRING=${BALLANCE_TEST_CONFIG}
        -P "${CMAKE_CURRENT_LIST_DIR}/RunStageInstall.cmake"
)

add_test(NAME StageLayout
        COMMAND "${CMAKE_COMMAND}"
        -DSTAGE_ROOT:PATH=${CMAKE_INSTALL_PREFIX}
        -DBALLANCE_BUILD_STATIC:BOOL=${BALLANCE_BUILD_STATIC}
        -DCHECK_ASSETS:BOOL=$<BOOL:${BALLANCE_ASSETS_ROOT}>
        -DCHECK_RENDER_CONFIGS:BOOL=${_ballance_check_render_configs}
        -DCHECK_SDL3_RUNTIME:BOOL=${_ballance_check_sdl3_runtime}
        -P "${CMAKE_CURRENT_LIST_DIR}/VerifyStage.cmake"
)

set_tests_properties(StageLayout PROPERTIES DEPENDS StageInstall)

if (_ballance_stage_is_multi_config)
    set_tests_properties(StageInstall StageLayout PROPERTIES
            CONFIGURATIONS "${BALLANCE_TEST_CONFIG}"
    )
endif ()
