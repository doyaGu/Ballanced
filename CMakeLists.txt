cmake_minimum_required(VERSION 3.25)

project(Ballanced
        VERSION 0.2.0
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
include(BallanceLinkCache)
include(BallanceVersion)
include(CTest)

if (BUILD_TESTING)
    add_test(NAME ApplicationNumericalIsolation
            COMMAND "${CMAKE_COMMAND}"
            "-DTEST_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
            "-DTEST_ENGINE_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/Source/NeMo2"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/Source/NeMo2/cmake/TestNumericalIsolation.cmake")
    add_test(NAME CMakeRegeneration
            COMMAND "${CMAKE_COMMAND}"
            "-DTEST_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
            "-DTEST_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}"
            "-DTEST_GENERATOR=${CMAKE_GENERATOR}"
            "-DTEST_PLATFORM=${CMAKE_GENERATOR_PLATFORM}"
            "-DTEST_TOOLSET=${CMAKE_GENERATOR_TOOLSET}"
            "-DTEST_CONFIG=$<CONFIG>"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/TestBuildRegeneration.cmake")
    add_test(NAME StageInstallScheduling
            COMMAND "${CMAKE_COMMAND}"
            "-DTEST_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
            "-DTEST_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}"
            "-DTEST_GENERATOR=${CMAKE_GENERATOR}"
            "-DTEST_PLATFORM=${CMAKE_GENERATOR_PLATFORM}"
            "-DTEST_TOOLSET=${CMAKE_GENERATOR_TOOLSET}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/TestStageInstallScheduling.cmake")
    set_tests_properties(StageInstallScheduling PROPERTIES TIMEOUT 90)
endif ()

add_subdirectory(Source)

include(BallanceStage)
