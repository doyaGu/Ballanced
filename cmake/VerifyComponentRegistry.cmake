cmake_minimum_required(VERSION 3.21)

if (NOT SOURCE_DIR)
    message(FATAL_ERROR "SOURCE_DIR is required")
endif ()

include("${SOURCE_DIR}/cmake/BallanceComponentRegistry.cmake")
set(_player_static_runtime_targets ${PLAYER_DECLARED_RUNTIME_TARGETS})

# Player declares the full registration catalogue, including optional modules.
# The selected runtime list omits disabled modules such as MIDI on non-Windows.
set(_expected_static_runtime_targets ${NEMO_SUPPORTED_MODULE_RUNTIME_TARGETS})
list(SORT _expected_static_runtime_targets)
list(SORT _player_static_runtime_targets)
if (NOT _player_static_runtime_targets STREQUAL _expected_static_runtime_targets)
    message(FATAL_ERROR
            "Player module declarations differ from the engine catalogue.\n"
            "  Player: ${_player_static_runtime_targets}\n"
            "  Engine: ${_expected_static_runtime_targets}")
endif ()

message(STATUS "[ComponentRegistry] Root and Player component declarations agree")
