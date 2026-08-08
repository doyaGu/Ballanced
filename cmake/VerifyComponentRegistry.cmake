cmake_minimum_required(VERSION 3.21)

if (NOT SOURCE_DIR)
    message(FATAL_ERROR "SOURCE_DIR is required")
endif ()

include("${SOURCE_DIR}/cmake/BallanceComponentRegistry.cmake")

set(_player_cmake_path "${SOURCE_DIR}/Source/Player/src/CMakeLists.txt")
set(_static_plugins_path "${SOURCE_DIR}/Source/Player/src/StaticPlugins.cpp")
file(READ "${_player_cmake_path}" _player_cmake)
file(READ "${_static_plugins_path}" _static_plugins)

# Every runtime module assembled by the root project must have a corresponding
# static target in Player's standalone/static composition list, and vice versa.
string(REGEX MATCHALL
        "_player_add_static_module\\([A-Za-z0-9_]+Static[ \t\r\n]+BALLANCE_STATIC_HAVE_[A-Z0-9_]+[ \t\r\n]+(TRUE|FALSE)\\)"
        _static_module_declarations
        "${_player_cmake}")

set(_player_static_runtime_targets)
set(_player_static_macros)
foreach (_declaration IN LISTS _static_module_declarations)
    string(REGEX REPLACE
            ".*_player_add_static_module\\(([A-Za-z0-9_]+)Static[ \t\r\n]+(BALLANCE_STATIC_HAVE_[A-Z0-9_]+)[ \t\r\n]+(TRUE|FALSE)\\).*"
            "\\1;\\2"
            _fields
            "${_declaration}")
    list(GET _fields 0 _runtime_target)
    list(GET _fields 1 _macro)
    list(APPEND _player_static_runtime_targets "${_runtime_target}")
    list(APPEND _player_static_macros "${_macro}")
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

# CKBgfxRasterizer is linked into CK2_3D but is not itself registered as a CK
# plugin. All other static composition macros must match the C++ registry.
set(_link_only_macros BALLANCE_STATIC_HAVE_CKBGFXRASTERIZER)
string(REGEX MATCHALL "BALLANCE_STATIC_HAVE_[A-Z0-9_]+" _source_macros "${_static_plugins}")
list(REMOVE_DUPLICATES _player_static_macros)
list(REMOVE_DUPLICATES _source_macros)
foreach (_macro IN LISTS _link_only_macros)
    list(REMOVE_ITEM _player_static_macros "${_macro}")
    list(REMOVE_ITEM _source_macros "${_macro}")
endforeach ()
list(SORT _player_static_macros)
list(SORT _source_macros)
if (NOT _player_static_macros STREQUAL _source_macros)
    message(FATAL_ERROR
            "Static plugin macros differ between Player CMake and StaticPlugins.cpp.\n"
            "  CMake:  ${_player_static_macros}\n"
            "  Source: ${_source_macros}")
endif ()

message(STATUS "[ComponentRegistry] Runtime and static component declarations agree")
