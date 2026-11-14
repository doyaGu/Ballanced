# NeMo2 owns engine declarations; Ballanced adds its application to that list.
include_guard(GLOBAL)

if (BALLANCE_COMPONENT_MANIFEST)
    include("${BALLANCE_COMPONENT_MANIFEST}")
else ()
    get_property(_nemo_manifest GLOBAL PROPERTY NEMO_RUNTIME_COMPONENT_MANIFEST)
    if (NOT _nemo_manifest OR NOT EXISTS "${_nemo_manifest}")
        message(FATAL_ERROR "NeMo2 did not publish its runtime component manifest")
    endif ()
    include("${_nemo_manifest}")
    get_property(PLAYER_DECLARED_RUNTIME_TARGETS GLOBAL PROPERTY PLAYER_RUNTIME_MODULES)
    get_property(PLAYER_REQUIRED_RUNTIME_TARGETS GLOBAL PROPERTY PLAYER_REQUIRED_RUNTIME_MODULES)
endif ()

# The engine may provide modules that this application does not consume.
foreach (_module IN LISTS PLAYER_DECLARED_RUNTIME_TARGETS)
    list(FIND NEMO_SUPPORTED_MODULE_RUNTIME_TARGETS "${_module}" _module_index)
    if (_module_index EQUAL -1)
        message(FATAL_ERROR "Player declares unsupported engine module: ${_module}")
    endif ()
endforeach ()
foreach (_module IN LISTS PLAYER_REQUIRED_RUNTIME_TARGETS)
    list(FIND PLAYER_DECLARED_RUNTIME_TARGETS "${_module}" _module_index)
    if (_module_index EQUAL -1)
        message(FATAL_ERROR "Player requires an undeclared engine module: ${_module}")
    endif ()
    list(FIND NEMO_MODULE_RUNTIME_TARGETS "${_module}" _module_index)
    if (_module_index EQUAL -1)
        message(FATAL_ERROR "Player requires an unavailable engine module: ${_module}")
    endif ()
endforeach ()

# Keep the staging script interface stable while taking every engine group
# from NeMo2's resolved manifest.
foreach (_group IN ITEMS
        CORE_RUNTIME_TARGETS RENDER_ENGINE_RUNTIME_TARGETS
        RENDER_ENGINE_DISABLED_OUTPUTS RENDER_ENGINE_RUNTIME_CONFIGS
        RENDER_ENGINE_RUNTIME_OUTPUTS MANAGER_RUNTIME_TARGETS MANAGER_RUNTIME_OUTPUTS
        PLUGIN_RUNTIME_TARGETS PLUGIN_RUNTIME_OUTPUTS
        BUILDING_BLOCK_RUNTIME_TARGETS BUILDING_BLOCK_REQUIRED_OUTPUTS
        BUILDING_BLOCK_OPTIONAL_OUTPUTS MODULE_RUNTIME_TARGETS)
    set(BALLANCE_${_group} "${NEMO_${_group}}")
endforeach ()
set(BALLANCE_RUNTIME_TARGETS ${NEMO_RUNTIME_TARGETS} Player)
