cmake_minimum_required(VERSION 3.21)

foreach (required TEST_SOURCE_DIR TEST_BINARY_DIR TEST_GENERATOR)
    if (NOT DEFINED ${required} OR "${${required}}" STREQUAL "")
        message(FATAL_ERROR "Missing ${required}")
    endif ()
endforeach ()

string(RANDOM LENGTH 12 ALPHABET 0123456789abcdef test_id)
set(test_root "${TEST_BINARY_DIR}/tests/cmake-regeneration/${test_id}")
set(test_source "${test_root}/source")
set(test_build "${test_root}/build")
file(MAKE_DIRECTORY "${test_source}")
file(TO_CMAKE_PATH "${TEST_SOURCE_DIR}/cmake/BallanceProject.cmake" policy_path)
file(WRITE "${test_source}/CMakeLists.txt"
        "cmake_minimum_required(VERSION 3.21)\n"
        "project(RegenerationCheck NONE)\n"
        "include(\"${policy_path}\")\n")

set(configure "${CMAKE_COMMAND}" -S "${test_source}" -B "${test_build}" -G "${TEST_GENERATOR}")
if (TEST_PLATFORM)
    list(APPEND configure -A "${TEST_PLATFORM}")
endif ()
if (TEST_TOOLSET)
    list(APPEND configure -T "${TEST_TOOLSET}")
endif ()
execute_process(COMMAND ${configure} RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error)
file(WRITE "${test_root}/configure.log" "${output}${error}")
if (NOT result EQUAL 0)
    message(FATAL_ERROR "Regeneration fixture configure failed: ${output}${error}")
endif ()

file(APPEND "${test_source}/CMakeLists.txt"
        "add_custom_target(Regenerated ALL COMMAND \"${CMAKE_COMMAND}\" -E touch \"${test_build}/regenerated\")\n")
set(build "${CMAKE_COMMAND}" --build "${test_build}")
if (TEST_CONFIG)
    list(APPEND build --config "${TEST_CONFIG}")
endif ()
execute_process(COMMAND ${build} RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error)
file(WRITE "${test_root}/build.log" "${output}${error}")
if (NOT result EQUAL 0 OR NOT EXISTS "${test_build}/regenerated")
    message(FATAL_ERROR "Build did not regenerate after CMakeLists.txt changed: ${output}${error}")
endif ()
