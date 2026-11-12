# Preserve the application's existing cache options.
if (DEFINED BALLANCE_ENABLE_LINK_CACHE)
    set(NEMO_ENABLE_LINK_CACHE "${BALLANCE_ENABLE_LINK_CACHE}")
endif ()
if (DEFINED BALLANCE_LINK_CACHE_DIR)
    set(NEMO_LINK_CACHE_DIR "${BALLANCE_LINK_CACHE_DIR}")
endif ()
include("${CMAKE_CURRENT_LIST_DIR}/../Source/NeMo2/cmake/NeMo2LinkCache.cmake")
