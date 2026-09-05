if (NOT DEFINED TEST_ROOT)
    message(FATAL_ERROR "TEST_ROOT is required")
endif ()
set(_stage "${TEST_ROOT}/build/stage")
file(MAKE_DIRECTORY "${_stage}/RenderEngines" "${_stage}/Bin")
foreach (_file IN ITEMS CKBgfxRasterizer.dll CKSdlGpuRasterizer.dll UserRasterizer.dll CKBgfxRasterizer.ini)
    file(WRITE "${_stage}/RenderEngines/${_file}" "test fixture: ${_file}")
endforeach ()
file(WRITE "${_stage}/Bin/libCKBgfxRasterizer.so" "old plugin")

function(_prune bgfx sdl static stage expect_success)
    execute_process(COMMAND "${CMAKE_COMMAND}"
        "-DBUILD_ROOT=${TEST_ROOT}/build" "-DSTAGE_ROOT=${stage}"
        "-DCKRE_BUILD_BGFX_RASTERIZER=${bgfx}" "-DCKRE_BUILD_SDL_GPU_RASTERIZER=${sdl}"
        "-DBALLANCE_BUILD_STATIC=${static}"
        -P "${CMAKE_CURRENT_LIST_DIR}/PruneDisabledRasterizers.cmake"
        RESULT_VARIABLE _result OUTPUT_VARIABLE _output ERROR_VARIABLE _error)
    if ((expect_success AND NOT _result EQUAL 0) OR (NOT expect_success AND _result EQUAL 0))
        message(FATAL_ERROR "Unexpected pruning outcome: ${_result}\n${_output}\n${_error}")
    endif ()
endfunction()

_prune(ON ON OFF "${_stage}" TRUE)
if (NOT EXISTS "${_stage}/RenderEngines/CKBgfxRasterizer.dll")
    message(FATAL_ERROR "Enabled bgfx was removed")
endif ()
_prune(OFF ON OFF "${_stage}" TRUE)
if (EXISTS "${_stage}/RenderEngines/CKBgfxRasterizer.dll" OR
        EXISTS "${_stage}/Bin/libCKBgfxRasterizer.so" OR
        NOT EXISTS "${_stage}/RenderEngines/CKSdlGpuRasterizer.dll")
    message(FATAL_ERROR "SDL-only stage contains the wrong rasterizers")
endif ()
file(WRITE "${_stage}/RenderEngines/CKBgfxRasterizer.dll" "re-enabled plugin")
_prune(ON OFF OFF "${_stage}" TRUE)
if (EXISTS "${_stage}/RenderEngines/CKSdlGpuRasterizer.dll" OR
        NOT EXISTS "${_stage}/RenderEngines/CKBgfxRasterizer.dll")
    message(FATAL_ERROR "bgfx-only stage contains the wrong rasterizers")
endif ()
_prune(ON ON ON "${_stage}" TRUE)
if (EXISTS "${_stage}/RenderEngines/CKBgfxRasterizer.dll")
    message(FATAL_ERROR "Static stage retains a dynamic rasterizer")
endif ()
foreach (_file IN ITEMS UserRasterizer.dll CKBgfxRasterizer.ini)
    file(READ "${_stage}/RenderEngines/${_file}" _content)
    if (NOT _content STREQUAL "test fixture: ${_file}")
        message(FATAL_ERROR "Unrelated plugin/configuration changed")
    endif ()
endforeach ()

file(MAKE_DIRECTORY "${TEST_ROOT}/external/RenderEngines")
file(WRITE "${TEST_ROOT}/external/RenderEngines/CKBgfxRasterizer.dll" "external plugin")
_prune(OFF ON OFF "${TEST_ROOT}/external" FALSE)
file(READ "${TEST_ROOT}/external/RenderEngines/CKBgfxRasterizer.dll" _external)
if (NOT _external STREQUAL "external plugin")
    message(FATAL_ERROR "External install was changed")
endif ()
