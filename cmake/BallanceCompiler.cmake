# Player and the engine must use the same MSVC runtime library.
set(NEMO_BUILD_STATIC "${BALLANCE_BUILD_STATIC}")
include("${CMAKE_CURRENT_LIST_DIR}/../Source/NeMo2/cmake/NeMo2Compiler.cmake")
