# Root-only staging, asset installation, and layout test registration.

include(CTest)
include(BallanceComponentRegistry)

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

foreach (_target IN LISTS BALLANCE_RUNTIME_TARGETS)
    if (TARGET ${_target})
        add_dependencies(stage ${_target})
    endif ()
endforeach ()

add_test(NAME ComponentRegistry
        COMMAND "${CMAKE_COMMAND}"
        -DSOURCE_DIR:PATH=${CMAKE_SOURCE_DIR}
        -P "${CMAKE_CURRENT_LIST_DIR}/VerifyComponentRegistry.cmake"
)

if (TARGET PlayerSdlShortcutsTest)
    add_dependencies(stage PlayerSdlShortcutsTest)
endif ()

set(_ballance_check_sdl3_runtime OFF)
if (_ballance_sdl3_runtime_target)
    set(_ballance_check_sdl3_runtime ON)
    if (APPLE)
        get_target_property(_ballance_sdl3_is_framework
                ${_ballance_sdl3_runtime_target} FRAMEWORK)
        if (_ballance_sdl3_is_framework)
            message(FATAL_ERROR
                    "SDL3 framework packages are not supported by the macOS stage layout; use a shared-library package")
        endif ()
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

if (BALLANCE_EFFECTIVE_ASSETS_ROOT AND EXISTS "${BALLANCE_EFFECTIVE_ASSETS_ROOT}")
    message(STATUS "[Ballance] Staging assets from: ${BALLANCE_EFFECTIVE_ASSETS_ROOT}")

    install(DIRECTORY
            "${BALLANCE_EFFECTIVE_ASSETS_ROOT}/Textures"
            "${BALLANCE_EFFECTIVE_ASSETS_ROOT}/Sounds"
            "${BALLANCE_EFFECTIVE_ASSETS_ROOT}/Text"
            "${BALLANCE_EFFECTIVE_ASSETS_ROOT}/3D Entities"
            DESTINATION . OPTIONAL
            COMPONENT Runtime
    )

    install(FILES
            "${BALLANCE_EFFECTIVE_ASSETS_ROOT}/base.cmo"
            "${BALLANCE_EFFECTIVE_ASSETS_ROOT}/Database.tdb"
            DESTINATION . OPTIONAL
            COMPONENT Runtime
    )
elseif (BALLANCE_EFFECTIVE_ASSETS_ROOT)
    message(WARNING "[Ballance] Asset path not found: ${BALLANCE_EFFECTIVE_ASSETS_ROOT}")
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
        -DCHECK_ASSETS:BOOL=$<BOOL:${BALLANCE_EFFECTIVE_ASSETS_ROOT}>
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
