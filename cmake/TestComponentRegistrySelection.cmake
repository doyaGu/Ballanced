cmake_minimum_required(VERSION 3.25)

set(fixture "${TEST_BINARY_DIR}/component-registry-selection")
file(MAKE_DIRECTORY "${fixture}")

function(check_registry name selected declared required expected_success)
    set(manifest "${fixture}/${name}.cmake")
    file(WRITE "${manifest}"
            "set(NEMO_SUPPORTED_MODULE_RUNTIME_TARGETS CK2_3D MidiManager NewEngineModule)\n"
            "set(NEMO_MODULE_RUNTIME_TARGETS \"${selected}\")\n"
            "set(PLAYER_DECLARED_RUNTIME_TARGETS \"${declared}\")\n"
            "set(PLAYER_REQUIRED_RUNTIME_TARGETS \"${required}\")\n")
    set(script "${TEST_SOURCE_DIR}/cmake/VerifyComponentRegistry.cmake")
    if (name STREQUAL "engine-extension")
        # Package checks also include the registry without a project policy scope.
        set(script "${fixture}/script-mode.cmake")
        file(WRITE "${script}" "include(\"${TEST_SOURCE_DIR}/cmake/BallanceComponentRegistry.cmake\")\n")
    endif ()
    execute_process(COMMAND "${CMAKE_COMMAND}"
            "-DSOURCE_DIR:PATH=${TEST_SOURCE_DIR}"
            "-DBALLANCE_COMPONENT_MANIFEST:FILEPATH=${manifest}"
            -P "${script}"
            RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error)
    if (expected_success AND NOT result EQUAL 0)
        message(FATAL_ERROR "${name} rejected matching catalogues:\n${output}${error}")
    elseif (NOT expected_success AND result EQUAL 0)
        message(FATAL_ERROR "${name} accepted mismatched catalogues")
    endif ()
endfunction()

# Optional platform modules remain declared even when omitted from this build.
check_registry(midi-disabled "CK2_3D" "CK2_3D;MidiManager" "CK2_3D" TRUE)
check_registry(midi-enabled "CK2_3D;MidiManager" "CK2_3D;MidiManager" "CK2_3D" TRUE)
check_registry(engine-extension "CK2_3D;NewEngineModule" "CK2_3D" "CK2_3D" TRUE)
check_registry(missing-required "MidiManager" "CK2_3D;MidiManager" "CK2_3D" FALSE)
check_registry(undeclared-required "CK2_3D" "MidiManager" "CK2_3D" FALSE)
check_registry(unsupported-declaration "CK2_3D" "CK2_3D;UnknownModule" "CK2_3D" FALSE)
