if (NOT DEFINED STAGE_ROOT OR NOT DEFINED BUILD_ROOT OR NOT DEFINED PLAYER_FILENAME)
    message(FATAL_ERROR "STAGE_ROOT, BUILD_ROOT, and PLAYER_FILENAME are required")
endif ()

set(_player "${STAGE_ROOT}/Bin/${PLAYER_FILENAME}")
set(_stage_config "${STAGE_ROOT}/Bin/Player.ini")
set(_base_cmo "${STAGE_ROOT}/base.cmo")
set(_stage_database "${STAGE_ROOT}/Database.tdb")
set(_output "${BUILD_ROOT}/player-level-entry-smoke")
set(_config "${_output}/Player.ini")
set(_log "${_output}/Player.log")
set(_capture "${_output}/level01-entry.bmp")

foreach (_required IN ITEMS "${_player}" "${_stage_config}" "${_base_cmo}" "${_stage_database}")
    if (NOT EXISTS "${_required}")
        message(FATAL_ERROR "Required staged file is missing: ${_required}")
    endif ()
endforeach ()

file(REMOVE_RECURSE "${_output}")
file(MAKE_DIRECTORY "${_output}")
configure_file("${_stage_config}" "${_config}" COPYONLY)
configure_file("${_stage_database}" "${_output}/Database.tdb" COPYONLY)

execute_process(
        COMMAND "${_player}"
        "--config=${_config}"
        "--log=${_log}"
        "--root-path=${STAGE_ROOT}"
        "--data-path=${_output}"
        "--cmo=${_base_cmo}"
        "--level-entry-smoke-output=${_output}"
        "--verbose"
        WORKING_DIRECTORY "${STAGE_ROOT}/Bin"
        TIMEOUT 75
        RESULT_VARIABLE _result
)

if (EXISTS "${_log}")
    file(READ "${_log}" _log_contents)
else ()
    set(_log_contents "(Player did not create a log file)")
endif ()

if (NOT _result EQUAL 0)
    message(FATAL_ERROR "Player Level 01 entry smoke failed (${_result}).\n${_log_contents}")
endif ()

if (NOT EXISTS "${_capture}")
    message(FATAL_ERROR "Level 01 entry capture is missing: ${_capture}")
endif ()

message(STATUS "Player Level 01 entry smoke passed")
message(STATUS "Artifacts: ${_output}")
