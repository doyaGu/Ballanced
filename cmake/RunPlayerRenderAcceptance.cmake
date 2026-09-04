if (NOT DEFINED STAGE_ROOT OR NOT DEFINED BUILD_ROOT OR NOT DEFINED PLAYER_FILENAME)
    message(FATAL_ERROR "STAGE_ROOT, BUILD_ROOT, and PLAYER_FILENAME are required")
endif ()

set(_player "${STAGE_ROOT}/Bin/${PLAYER_FILENAME}")
set(_stage_config "${STAGE_ROOT}/Bin/Player.ini")
set(_base_cmo "${STAGE_ROOT}/base.cmo")
set(_stage_database "${STAGE_ROOT}/Database.tdb")
string(TIMESTAMP _session_stamp "%Y%m%d-%H%M%S")
set(_output "${BUILD_ROOT}/player-render-acceptance/${_session_stamp}")
set(_config "${_output}/Player.ini")
set(_log "${_output}/Player.log")
set(_report "${_output}/acceptance-report.txt")

foreach (_required IN ITEMS "${_player}" "${_stage_config}" "${_base_cmo}" "${_stage_database}")
    if (NOT EXISTS "${_required}")
        message(FATAL_ERROR "Required staged file is missing: ${_required}")
    endif ()
endforeach ()

file(MAKE_DIRECTORY "${_output}")
configure_file("${_stage_config}" "${_config}" COPYONLY)
configure_file("${_stage_database}" "${_output}/Database.tdb" COPYONLY)

message(STATUS "Starting visible manual render acceptance")
message(STATUS "Play normally and close Ballance when inspection is complete")
execute_process(
        COMMAND "${_player}"
        "--config=${_config}"
        "--log=${_log}"
        "--root-path=${STAGE_ROOT}"
        "--data-path=${_output}"
        "--cmo=${_base_cmo}"
        "--render-acceptance-output=${_output}"
        "--verbose"
        WORKING_DIRECTORY "${STAGE_ROOT}/Bin"
        RESULT_VARIABLE _result
)

if (NOT _result EQUAL 0)
    message(FATAL_ERROR "Player render acceptance session failed (${_result}); see ${_log}")
endif ()
if (NOT EXISTS "${_report}")
    message(FATAL_ERROR "Render acceptance report is missing: ${_report}")
endif ()

file(READ "${_report}" _report_contents)
message(STATUS "Render acceptance evidence: ${_output}")
message("${_report_contents}")
