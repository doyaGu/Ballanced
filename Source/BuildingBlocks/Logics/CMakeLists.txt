# Logics

add_library(Logics SHARED Logics.cpp Logics.rc)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Behaviors LOGICS_BEHAVIORS_SRCS)
target_sources(Logics PRIVATE ${LOGICS_BEHAVIORS_SRCS})
target_include_directories(Logics PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Logics PRIVATE CK2 VxMath)

install(TARGETS Logics RUNTIME DESTINATION BuildingBlocks)
