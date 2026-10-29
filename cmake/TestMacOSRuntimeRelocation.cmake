# Exercise the installed runtime after moving it away from its build prefix.
if (NOT DEFINED BUILD_DIR OR NOT DEFINED STAGE_ROOT)
    message(FATAL_ERROR "BUILD_DIR and STAGE_ROOT are required")
endif ()
set(_test_root "${BUILD_DIR}/macos-runtime-relocation")
file(REMOVE_RECURSE "${_test_root}")
set(_relocated_root "${_test_root}/relocated runtime")
file(MAKE_DIRECTORY "${_relocated_root}")
file(COPY "${STAGE_ROOT}/" DESTINATION "${_relocated_root}")
set(STAGE_ROOT "${_relocated_root}")
include("${CMAKE_CURRENT_LIST_DIR}/VerifyStage.cmake")

find_program(_codesign codesign REQUIRED)
find_program(_install_name_tool install_name_tool REQUIRED)
file(GLOB _sdl_candidates LIST_DIRECTORIES FALSE
        "${STAGE_ROOT}/Bin/libSDL3*.dylib" "${STAGE_ROOT}/Bin/SDL3*.dylib")
foreach (_candidate IN LISTS _sdl_candidates)
    if (NOT IS_SYMLINK "${_candidate}")
        set(_sdl_path "${_candidate}")
    endif ()
endforeach ()
get_filename_component(_sdl_name "${_sdl_path}" NAME)

# Simulate a package prefix containing spaces. The validator must reject that
# dependency even when the bundled SDL3's own install name is already correct.
set(_original_sdl "${_test_root}/original SDL/${_sdl_name}")
file(MAKE_DIRECTORY "${_test_root}/original SDL")
file(COPY "${_sdl_path}" DESTINATION "${_test_root}/original SDL")
file(SHA256 "${_original_sdl}" _original_hash)
execute_process(COMMAND "${_install_name_tool}" -change "@rpath/${_sdl_name}"
        "${_original_sdl}" "${STAGE_ROOT}/Bin/Player" COMMAND_ERROR_IS_FATAL ANY)
execute_process(COMMAND "${CMAKE_COMMAND}"
        "-DSTAGE_ROOT:PATH=${STAGE_ROOT}"
        "-DBALLANCE_COMPONENT_MANIFEST:FILEPATH=${BALLANCE_COMPONENT_MANIFEST}"
        "-DBALLANCE_BUILD_STATIC:BOOL=${BALLANCE_BUILD_STATIC}"
        -P "${CMAKE_CURRENT_LIST_DIR}/VerifyStage.cmake"
        RESULT_VARIABLE _bad_dependency_result OUTPUT_QUIET ERROR_VARIABLE _bad_dependency_error)
if (_bad_dependency_result EQUAL 0 OR NOT _bad_dependency_error MATCHES "Non-relocatable SDL3 dependency")
    message(FATAL_ERROR "Validator did not reject the absolute SDL3 dependency: ${_bad_dependency_error}")
endif ()
execute_process(COMMAND "${_install_name_tool}" -id "${_original_sdl}" "${_sdl_path}"
        COMMAND_ERROR_IS_FATAL ANY)
set(CMAKE_INSTALL_PREFIX "${STAGE_ROOT}")
include("${CMAKE_CURRENT_LIST_DIR}/RelocateMacOSRuntime.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/VerifyStage.cmake")
file(SHA256 "${_original_sdl}" _after_hash)
if (NOT _after_hash STREQUAL _original_hash)
    message(FATAL_ERROR "Relocation modified the original SDL3 library")
endif ()

set(_binaries "${STAGE_ROOT}/Bin/Player")
foreach (_directory IN ITEMS Bin RenderEngines Managers Plugins BuildingBlocks)
    file(GLOB _libraries LIST_DIRECTORIES FALSE "${STAGE_ROOT}/${_directory}/*.dylib")
    list(APPEND _binaries ${_libraries})
endforeach ()
set(_hashes "")
foreach (_binary IN LISTS _binaries)
    if (NOT IS_SYMLINK "${_binary}")
        execute_process(COMMAND "${_codesign}" --verify --strict "${_binary}"
                COMMAND_ERROR_IS_FATAL ANY)
        file(SHA256 "${_binary}" _hash)
        list(APPEND _hashes "${_hash}")
    endif ()
endforeach ()
include("${CMAKE_CURRENT_LIST_DIR}/RelocateMacOSRuntime.cmake")
set(_after_hashes "")
foreach (_binary IN LISTS _binaries)
    if (NOT IS_SYMLINK "${_binary}")
        file(SHA256 "${_binary}" _hash)
        list(APPEND _after_hashes "${_hash}")
    endif ()
endforeach ()
if (NOT _after_hashes STREQUAL _hashes)
    message(FATAL_ERROR "Relocating an already relocatable runtime changed its binaries")
endif ()

# An invalid selector exits before graphics/game initialization. Use a config
# inside the copied runtime so this never reads or writes the user's config.
execute_process(
        COMMAND "${CMAKE_COMMAND}" -E env "DYLD_PRINT_LIBRARIES=1"
                "${STAGE_ROOT}/Bin/Player" --config "${_test_root}/Player.ini"
                --rasterizer relocation-test-invalid
        WORKING_DIRECTORY "${STAGE_ROOT}"
        RESULT_VARIABLE _result
        OUTPUT_VARIABLE _output
        ERROR_VARIABLE _errors
        TIMEOUT 15)
if (NOT _result EQUAL 2 OR NOT _errors MATCHES "Invalid rasterizer selection")
    message(FATAL_ERROR "Relocated Player did not reach option parsing: ${_result}\n${_output}\n${_errors}")
endif ()
string(FIND "${_errors}" "${_sdl_path}" _loaded_staged_sdl)
if (_loaded_staged_sdl LESS 0)
    message(FATAL_ERROR "Relocated Player did not load the bundled SDL3:\n${_errors}")
endif ()
message(STATUS "Relocated Player loaded its bundled SDL3 and reached option parsing")
