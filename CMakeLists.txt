cmake_minimum_required(VERSION 3.21)

project(Ballanced
        VERSION 0.1.0
        DESCRIPTION "Ballance Decompilation"
        HOMEPAGE_URL https://github.com/doyaGu/Ballanced
        LANGUAGES C CXX
)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(GNUInstallDirs)
include(BallanceCache)
include(BallanceProject)
include(BallanceOptions)
include(BallanceCompiler)
include(BallanceVersion)
include(CTest)

if (BUILD_TESTING)
    add_test(NAME NumericalIsolation
            COMMAND "${CMAKE_COMMAND}"
            "-DTEST_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/TestNumericalIsolation.cmake")
    set_tests_properties(NumericalIsolation PROPERTIES
            SKIP_REGULAR_EXPRESSION "Numerical isolation requires a Git checkout")
    add_test(NAME NumericalIsolationRules
            COMMAND "${CMAKE_COMMAND}"
            "-DTEST_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
            "-DTEST_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}/Testing"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/TestNumericalIsolationRules.cmake")
    set_tests_properties(NumericalIsolationRules PROPERTIES
            SKIP_REGULAR_EXPRESSION "Numerical isolation requires Git")
    add_test(NAME CMakeRegeneration
            COMMAND "${CMAKE_COMMAND}"
            "-DTEST_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
            "-DTEST_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}"
            "-DTEST_GENERATOR=${CMAKE_GENERATOR}"
            "-DTEST_PLATFORM=${CMAKE_GENERATOR_PLATFORM}"
            "-DTEST_TOOLSET=${CMAKE_GENERATOR_TOOLSET}"
            "-DTEST_CONFIG=$<CONFIG>"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/TestBuildRegeneration.cmake")
endif ()

add_subdirectory(Source)

include(BallanceStage)
