# Relocate and sign the installed engine libraries together with Player.
set(NEMO_RUNTIME_EXECUTABLES Bin/Player)
include("${CMAKE_CURRENT_LIST_DIR}/../Source/NeMo2/cmake/RelocateMacOSRuntime.cmake")
