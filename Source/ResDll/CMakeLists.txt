# ResDll

if(NOT DEFINED RESDLL_NAME)
    set(RESDLL_NAME "ResDll")
endif()

set(RESDLL_HEADERS ResDll.h)

set(RESDLL_SOURCES ResDll.c ResDll.rc ResDll.def)

add_library(ResDll SHARED ${RESDLL_HEADERS} ${RESDLL_SOURCES})
set_target_properties(ResDll PROPERTIES
    OUTPUT_NAME ${RESDLL_NAME}
)
target_include_directories(ResDll PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_definitions(ResDll PRIVATE "RESDLL_EXPORTS")

copy_target_to_ballance_dir(ResDll "Bin")

install(TARGETS ResDll RUNTIME DESTINATION Bin)