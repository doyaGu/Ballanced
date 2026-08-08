# Root-only options and child-project defaults for the superproject.

ballance_set_cache_default(BALLANCE_AUTO_DETECT_ASSETS OFF BOOL
        "Use the repository-local assets directory for staging when it exists")
ballance_set_cache_default(BALLANCE_ASSETS_ROOT "" PATH
        "Path to Ballance game root for asset staging (base.cmo, Textures/, etc.)")
ballance_set_cache_default(BALLANCE_ASSETS_ROOT_AUTO_DETECTED OFF INTERNAL
        "Whether BALLANCE_ASSETS_ROOT was populated by repository-local auto-detection")

set(_ballance_local_assets_root "${PROJECT_SOURCE_DIR}/assets")
if (BALLANCE_AUTO_DETECT_ASSETS AND NOT BALLANCE_ASSETS_ROOT AND EXISTS "${_ballance_local_assets_root}")
    set(BALLANCE_ASSETS_ROOT "${_ballance_local_assets_root}" CACHE PATH
            "Path to Ballance game root for asset staging (base.cmo, Textures/, etc.)"
            FORCE)
    set(BALLANCE_ASSETS_ROOT_AUTO_DETECTED ON CACHE INTERNAL
            "Whether BALLANCE_ASSETS_ROOT was populated by repository-local auto-detection"
            FORCE)
elseif (NOT BALLANCE_AUTO_DETECT_ASSETS AND
        (BALLANCE_ASSETS_ROOT_AUTO_DETECTED OR
         "${BALLANCE_ASSETS_ROOT}" STREQUAL "${_ballance_local_assets_root}"))
    # Clear values left in an existing cache by the former default-on behavior.
    set(BALLANCE_ASSETS_ROOT "" CACHE PATH
            "Path to Ballance game root for asset staging (base.cmo, Textures/, etc.)"
            FORCE)
    set(BALLANCE_ASSETS_ROOT_AUTO_DETECTED OFF CACHE INTERNAL
            "Whether BALLANCE_ASSETS_ROOT was populated by repository-local auto-detection"
            FORCE)
elseif (BALLANCE_ASSETS_ROOT_AUTO_DETECTED AND
        NOT "${BALLANCE_ASSETS_ROOT}" STREQUAL "${_ballance_local_assets_root}")
    set(BALLANCE_ASSETS_ROOT_AUTO_DETECTED OFF CACHE INTERNAL
            "Whether BALLANCE_ASSETS_ROOT was populated by repository-local auto-detection"
            FORCE)
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
