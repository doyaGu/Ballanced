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

ballance_set_cache_default(BALLANCE_DIR "" PATH
        "Optional: Ballance game directory for manual deployment")

ballance_set_cache_default(BALLANCE_TEST_CONFIG "Release" STRING
        "CTest configuration for multi-config generators")

ballance_set_cache_default(BALLANCE_BUILD_STATIC OFF BOOL
        "Build Player with Virtools modules linked statically")

ballance_set_cache_default(BALLANCE_TARGET_ARCH "" STRING
        "Optional target architecture label used by platform presets")

find_package(SDL3 CONFIG REQUIRED)

foreach (_component IN ITEMS VXMATH CK2 CKRE)
    if (BALLANCE_BUILD_STATIC)
        ballance_set_cache_default(${_component}_BUILD_SHARED OFF BOOL "")
        ballance_set_cache_default(${_component}_BUILD_STATIC ON BOOL "")
    else ()
        ballance_set_cache_default(${_component}_BUILD_SHARED ON BOOL "")
        ballance_set_cache_default(${_component}_BUILD_STATIC OFF BOOL "")
    endif ()
    ballance_set_cache_default(${_component}_INSTALL ON BOOL "")
    ballance_set_cache_default(${_component}_BUILD_TESTS OFF BOOL "")
endforeach ()

if (BALLANCE_BUILD_STATIC)
    ballance_set_cache_default(CKBB_BUILD_SHARED OFF BOOL "")
    ballance_set_cache_default(CKBB_BUILD_STATIC ON BOOL "")
    ballance_set_cache_default(CKPLUGINS_BUILD_SHARED OFF BOOL "")
    ballance_set_cache_default(CKPLUGINS_BUILD_STATIC ON BOOL "")
    foreach (_mgr IN ITEMS SDLINPUT SDLSOUND CKPARAMOP)
        ballance_set_cache_default(${_mgr}_BUILD_SHARED OFF BOOL "")
        ballance_set_cache_default(${_mgr}_BUILD_STATIC ON BOOL "")
    endforeach ()
else ()
    ballance_set_cache_default(CKBB_BUILD_SHARED ON BOOL "")
    ballance_set_cache_default(CKBB_BUILD_STATIC OFF BOOL "")
    ballance_set_cache_default(CKPLUGINS_BUILD_SHARED ON BOOL "")
    foreach (_mgr IN ITEMS SDLINPUT SDLSOUND CKPARAMOP)
        ballance_set_cache_default(${_mgr}_BUILD_SHARED ON BOOL "")
    endforeach ()
endif ()
ballance_set_cache_default(CKBB_INSTALL ON BOOL "")
ballance_set_cache_default(CKBB_BUILD_TESTS OFF BOOL "")

ballance_set_cache_default(CKPLUGINS_INSTALL ON BOOL "")
ballance_set_cache_default(CKPLUGINS_BUILD_TESTS OFF BOOL "")

foreach (_mgr IN ITEMS SDLINPUT SDLSOUND CKPARAMOP)
    ballance_set_cache_default(${_mgr}_INSTALL ON BOOL "")
endforeach ()

ballance_set_cache_default(CKRE_BUILD_BGFX_RASTERIZER ON BOOL "")
if (NOT WIN32)
    ballance_set_cache_default(CKBB_BUILD_MidiManager OFF BOOL "")
endif ()
