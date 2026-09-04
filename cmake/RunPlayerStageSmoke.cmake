if (NOT DEFINED STAGE_ROOT OR NOT DEFINED BUILD_ROOT OR NOT DEFINED PLAYER_FILENAME)
    message(FATAL_ERROR "STAGE_ROOT, BUILD_ROOT, and PLAYER_FILENAME are required")
endif ()

set(_player "${STAGE_ROOT}/Bin/${PLAYER_FILENAME}")
set(_stage_config "${STAGE_ROOT}/Bin/Player.ini")
set(_base_cmo "${STAGE_ROOT}/base.cmo")
set(_output "${BUILD_ROOT}/player-stage-smoke")
set(_config "${_output}/Player.ini")
set(_log "${_output}/Player.log")
set(_capture "${_output}/main-menu.bmp")

foreach (_required IN ITEMS "${_player}" "${_stage_config}" "${_base_cmo}")
    if (NOT EXISTS "${_required}")
        message(FATAL_ERROR "Required staged file is missing: ${_required}")
    endif ()
endforeach ()

file(REMOVE_RECURSE "${_output}")
file(MAKE_DIRECTORY "${_output}")
configure_file("${_stage_config}" "${_config}" COPYONLY)

execute_process(
        COMMAND "${_player}"
        "--config=${_config}"
        "--log=${_log}"
        "--root-path=${STAGE_ROOT}"
        "--cmo=${_base_cmo}"
        "--stage-smoke-output=${_output}"
        "--verbose"
        WORKING_DIRECTORY "${STAGE_ROOT}/Bin"
        TIMEOUT 60
        RESULT_VARIABLE _result
)

if (EXISTS "${_log}")
    file(READ "${_log}" _log_contents)
else ()
    set(_log_contents "(Player did not create a log file)")
endif ()

if (NOT _result EQUAL 0)
    message(FATAL_ERROR "Player stage smoke failed (${_result}).\n${_log_contents}")
endif ()

if (NOT EXISTS "${_capture}")
    message(FATAL_ERROR "Stage smoke capture is missing: ${_capture}")
endif ()

message(STATUS "Player stage smoke passed")
message(STATUS "Artifacts: ${_output}")
