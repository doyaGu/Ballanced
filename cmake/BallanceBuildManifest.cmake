# Generate a machine-readable record of the exact component revisions used by
# this assembled runtime. Top-level component commits also pin their nested
# submodules.

find_package(Git QUIET)

function(_ballance_read_revision relative_path variable_prefix)
    set(_revision "unknown")
    set(_dirty true)
    set(_source_path "${CMAKE_SOURCE_DIR}/${relative_path}")

    if (GIT_FOUND AND EXISTS "${_source_path}")
        execute_process(
                COMMAND "${GIT_EXECUTABLE}" -C "${_source_path}" rev-parse HEAD
                RESULT_VARIABLE _revision_result
                OUTPUT_VARIABLE _revision_output
                ERROR_QUIET
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if (_revision_result EQUAL 0)
            set(_revision "${_revision_output}")
        endif ()

        execute_process(
                COMMAND "${GIT_EXECUTABLE}" -C "${_source_path}" status --porcelain --untracked-files=no
                RESULT_VARIABLE _status_result
                OUTPUT_VARIABLE _status_output
                ERROR_QUIET
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if (_status_result EQUAL 0 AND _status_output STREQUAL "")
            set(_dirty false)
        endif ()
    endif ()

    set(${variable_prefix}_REVISION "${_revision}" PARENT_SCOPE)
    set(${variable_prefix}_DIRTY "${_dirty}" PARENT_SCOPE)
endfunction()

_ballance_read_revision("." BALLANCE_ROOT)
_ballance_read_revision("Source/Player" BALLANCE_PLAYER)
_ballance_read_revision("Source/VxMath" BALLANCE_VXMATH)
_ballance_read_revision("Source/CK2" BALLANCE_CK2)
_ballance_read_revision("Source/RenderEngine" BALLANCE_RENDER_ENGINE)
_ballance_read_revision("Source/BuildingBlocks" BALLANCE_BUILDING_BLOCKS)
_ballance_read_revision("Source/Plugins" BALLANCE_PLUGINS)
_ballance_read_revision("Source/Managers/ParameterOperations" BALLANCE_PARAMETER_OPERATIONS)
_ballance_read_revision("Source/Managers/SdlInputManager" BALLANCE_SDL_INPUT_MANAGER)
_ballance_read_revision("Source/Managers/SdlSoundManager" BALLANCE_SDL_SOUND_MANAGER)

set(_ballance_manifest_dir "${CMAKE_BINARY_DIR}/generated")
file(MAKE_DIRECTORY "${_ballance_manifest_dir}")
set(BALLANCE_BUILD_MANIFEST "${_ballance_manifest_dir}/BallancedBuildManifest.json")
configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/BallancedBuildManifest.json.in"
        "${BALLANCE_BUILD_MANIFEST}"
        @ONLY
)

install(FILES "${BALLANCE_BUILD_MANIFEST}"
        DESTINATION Bin
        COMPONENT Runtime
)
