include("${CMAKE_CURRENT_LIST_DIR}/../Source/NeMo2/cmake/NeMo2Cache.cmake")

function(ballance_set_cache_default var value type doc)
    nemo_set_cache_default("${var}" "${value}" "${type}" "${doc}")
endfunction()
