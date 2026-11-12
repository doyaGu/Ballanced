# Root-only options and child-project defaults for the superproject.

ballance_set_cache_default(BALLANCE_AUTO_DETECT_ASSETS OFF BOOL
        "Use the repository-local assets directory for staging when it exists")
ballance_set_cache_default(BALLANCE_ASSETS_ROOT "" PATH
        "Path to Ballance game root for asset staging (base.cmo, Textures/, etc.)")

set(_ballance_local_assets_root "${PROJECT_SOURCE_DIR}/assets")
set(BALLANCE_EFFECTIVE_ASSETS_ROOT "${BALLANCE_ASSETS_ROOT}")
if (BALLANCE_AUTO_DETECT_ASSETS AND NOT BALLANCE_ASSETS_ROOT AND EXISTS "${_ballance_local_assets_root}")
    set(BALLANCE_EFFECTIVE_ASSETS_ROOT "${_ballance_local_assets_root}")
endif ()
if (BALLANCE_EFFECTIVE_ASSETS_ROOT)
    file(TO_CMAKE_PATH "${BALLANCE_EFFECTIVE_ASSETS_ROOT}"
            BALLANCE_EFFECTIVE_ASSETS_ROOT)
endif ()

ballance_set_cache_default(BALLANCE_DIR "" PATH
        "Optional: Ballance game directory for manual deployment")

ballance_set_cache_default(BALLANCE_TEST_CONFIG "Release" STRING
        "CTest configuration for multi-config generators")

ballance_set_cache_default(BALLANCE_BUILD_STATIC OFF BOOL
        "Build Player with Virtools modules linked statically")

ballance_set_cache_default(BALLANCE_TARGET_ARCH "" STRING
        "Optional target architecture label used by platform presets")

# NeMo2 owns all engine module defaults and test dependency selection.
find_package(SDL3 3.4.20 CONFIG REQUIRED)
set(NEMO_BUILD_STATIC "${BALLANCE_BUILD_STATIC}")
set(NEMO_TARGET_ARCH "${BALLANCE_TARGET_ARCH}")
