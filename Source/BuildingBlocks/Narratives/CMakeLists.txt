# Narratives

add_library(Narratives SHARED Narratives.cpp Narratives.rc)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Behaviors NARRATIVES_BEHAVIORS_SRCS)
target_sources(Narratives PRIVATE ${NARRATIVES_BEHAVIORS_SRCS})
target_include_directories(Narratives PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Narratives PRIVATE CK2 VxMath)
target_compile_definitions(Narratives PRIVATE VX_API)

install(TARGETS Narratives RUNTIME DESTINATION BuildingBlocks)