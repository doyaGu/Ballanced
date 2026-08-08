cmake_minimum_required(VERSION 3.21)

if (NOT SOURCE_DIR)
    message(FATAL_ERROR "SOURCE_DIR is required")
endif ()

include("${SOURCE_DIR}/cmake/BallanceComponentRegistry.cmake")
include("${SOURCE_DIR}/Source/Player/cmake/PlayerStaticModules.cmake")

set(_player_static_runtime_targets)
foreach (_player_static_module IN LISTS PLAYER_STATIC_MODULES)
    set(_module_prefix "PLAYER_STATIC_MODULE_${_player_static_module}")
    set(_runtime_target "${${_module_prefix}_RUNTIME_TARGET}")
    list(APPEND _player_static_runtime_targets "${_runtime_target}")
endforeach ()

set(_expected_static_runtime_targets ${BALLANCE_MODULE_RUNTIME_TARGETS})
list(SORT _expected_static_runtime_targets)
list(SORT _player_static_runtime_targets)
if (NOT _player_static_runtime_targets STREQUAL _expected_static_runtime_targets)
    message(FATAL_ERROR
            "Static module targets differ from the root runtime registry.\n"
            "  Player: ${_player_static_runtime_targets}\n"
            "  Root:   ${_expected_static_runtime_targets}")
endif ()

message(STATUS "[ComponentRegistry] Root and Player component declarations agree")
