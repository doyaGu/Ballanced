cmake_minimum_required(VERSION 3.16)

project(Ballanced
        VERSION 0.1.0
        DESCRIPTION "Ballance Decompilation"
        HOMEPAGE_URL https://github.com/doyaGu/Ballanced
        LANGUAGES C CXX
)

# --- Platform checks ---
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not allowed. Use: cmake -B build")
endif ()

if (NOT WIN32)
    message(FATAL_ERROR "Only Windows is supported.")
endif ()

# --- C++ standard ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS YES)

# --- CMake modules ---
list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(GNUInstallDirs)
include(CTest)

# --- Helper: set cache variable with default ---
function(_set_cache_default var value type doc)
    if (NOT DEFINED ${var})
        set(${var} "${value}" CACHE ${type} "${doc}")
    endif ()
endfunction()

# --- Global settings ---
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (WIN32)
    set(CMAKE_USE_RELATIVE_PATHS ON)
    set(CMAKE_SUPPRESS_REGENERATION ON)
endif ()

# --- Build configuration ---
get_property(_is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if (NOT _is_multi_config AND NOT CMAKE_BUILD_TYPE)
    message(STATUS "[Ballanced] Setting build type to 'Release'")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif ()

# --- User options ---
_set_cache_default(BALLANCED_BUILD_ROOT "${CMAKE_BINARY_DIR}/Ballance" PATH
        "Build-tree output root (Bin/, Managers/, RenderEngines/, etc.)")

_set_cache_default(BALLANCED_ASSETS_ROOT "" PATH
        "Path to Ballance game root for asset staging (base.cmo, Textures/, etc.)")

_set_cache_default(BALLANCE_DIR "" PATH
        "Optional: Ballance game directory for manual deployment")

_set_cache_default(BALLANCED_TEST_CONFIG "Release" STRING
        "CTest configuration for multi-config generators")

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/stage" CACHE PATH "Staging root" FORCE)
    message(STATUS "[Ballanced] Install prefix: ${CMAKE_INSTALL_PREFIX}")
endif ()

# --- Compiler settings ---
add_compile_definitions(
        $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
        $<$<C_COMPILER_ID:MSVC>:_CRT_NONSTDC_NO_WARNINGS>
        $<$<CONFIG:Debug>:DEBUG>
)

# --- Visual Studio specific settings ---
if (MSVC)
    # Use ANSI character set (not UNICODE)
    add_compile_definitions(_MBCS)
    # Remove UNICODE defines if set
    remove_definitions(-DUNICODE -D_UNICODE)

    # Better Visual Studio experience
    set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD ON)

    # Set warning level
    if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W3" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else ()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
    endif ()

    # Multi-processor compilation
    add_compile_options(/MP)

    # Use dynamic CRT (/MD, /MDd) for all configurations unless user/toolchain overrides.
    _set_cache_default(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL" STRING
            "MSVC runtime library (default: /MD, /MDd)")
endif ()

# --- Output directories (Ballance runtime layout) ---
_set_cache_default(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BALLANCED_BUILD_ROOT}/Bin" PATH "")
_set_cache_default(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BALLANCED_BUILD_ROOT}/Bin" PATH "")
_set_cache_default(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib" PATH "")

foreach (_cfg IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
    _set_cache_default(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${_cfg} "${BALLANCED_BUILD_ROOT}/Bin" PATH "")
    _set_cache_default(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${_cfg} "${BALLANCED_BUILD_ROOT}/Bin" PATH "")
    _set_cache_default(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${_cfg} "${CMAKE_BINARY_DIR}/lib" PATH "")
endforeach ()

# --- Component options ---
foreach (_component IN ITEMS VXMATH CK2 CKRE)
    _set_cache_default(${_component}_BUILD_SHARED ON BOOL "")
    _set_cache_default(${_component}_BUILD_STATIC OFF BOOL "")
    _set_cache_default(${_component}_INSTALL ON BOOL "")
    _set_cache_default(${_component}_BUILD_TESTS OFF BOOL "")
endforeach ()

_set_cache_default(CKBB_BUILD_SHARED ON BOOL "")
_set_cache_default(CKBB_BUILD_STATIC OFF BOOL "")
_set_cache_default(CKBB_INSTALL ON BOOL "")

_set_cache_default(CKPLUGINS_INSTALL ON BOOL "")
_set_cache_default(CKPLUGINS_BUILD_TESTS OFF BOOL "")

foreach (_mgr IN ITEMS DX8INPUT DX8SOUND CKPARAMOP)
    _set_cache_default(${_mgr}_INSTALL ON BOOL "")
endforeach ()

# --- Add components ---
add_subdirectory(Source)

# --- Staging target ---
if (_is_multi_config)
    set(_install_cmd "${CMAKE_COMMAND}" --install "${CMAKE_BINARY_DIR}" --config "$<CONFIG>")
else ()
    set(_install_cmd "${CMAKE_COMMAND}" --install "${CMAKE_BINARY_DIR}")
endif ()

add_custom_target(stage
        COMMAND ${_install_cmd}
        COMMENT "Installing to ${CMAKE_INSTALL_PREFIX}"
        USES_TERMINAL
        VERBATIM
)

# Add runtime target dependencies
set(_runtime_targets
        VxMath CK2 CK2_3D CKDX9Rasterizer
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

foreach (_target IN LISTS _runtime_targets)
    if (TARGET ${_target})
        add_dependencies(stage ${_target})
    endif ()
endforeach ()

# --- Asset staging ---
if (BALLANCED_ASSETS_ROOT AND EXISTS "${BALLANCED_ASSETS_ROOT}")
    message(STATUS "[Ballanced] Staging assets from: ${BALLANCED_ASSETS_ROOT}")

    install(DIRECTORY
            "${BALLANCED_ASSETS_ROOT}/Textures"
            "${BALLANCED_ASSETS_ROOT}/Sounds"
            "${BALLANCED_ASSETS_ROOT}/Text"
            "${BALLANCED_ASSETS_ROOT}/3D Entities"
            DESTINATION . OPTIONAL
    )

    install(FILES
            "${BALLANCED_ASSETS_ROOT}/base.cmo"
            "${BALLANCED_ASSETS_ROOT}/Database.tdb"
            DESTINATION . OPTIONAL
    )
elseif (BALLANCED_ASSETS_ROOT)
    message(WARNING "[Ballanced] Asset path not found: ${BALLANCED_ASSETS_ROOT}")
endif ()

if (BALLANCE_DIR AND EXISTS "${BALLANCE_DIR}")
    message(STATUS "[Ballanced] Deploy target: ${BALLANCE_DIR}")
    message(STATUS "  Sync ${CMAKE_INSTALL_PREFIX} manually after staging")
elseif (BALLANCE_DIR)
    message(NOTICE "[Ballanced] Deploy path not found: ${BALLANCE_DIR}")
endif ()

# --- Tests ---
add_test(NAME StageInstall
        COMMAND "${CMAKE_COMMAND}"
        -DBUILD_DIR:PATH=${CMAKE_BINARY_DIR}
        -DCONFIG:STRING=${BALLANCED_TEST_CONFIG}
        -P "${CMAKE_CURRENT_LIST_DIR}/cmake/RunStageInstall.cmake"
)

add_test(NAME StageLayout
        COMMAND "${CMAKE_COMMAND}"
        -DSTAGE_ROOT:PATH=${CMAKE_INSTALL_PREFIX}
        -DCHECK_ASSETS:BOOL=$<BOOL:${BALLANCED_ASSETS_ROOT}>
        -P "${CMAKE_CURRENT_LIST_DIR}/cmake/VerifyStage.cmake"
)

set_tests_properties(StageLayout PROPERTIES DEPENDS StageInstall)

if (_is_multi_config)
    set_tests_properties(StageInstall StageLayout PROPERTIES
            CONFIGURATIONS "${BALLANCED_TEST_CONFIG}"
    )
endif ()