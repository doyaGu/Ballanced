# Lights

add_library(Lights SHARED Lights.cpp Lights.rc)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Behaviors LIGHTS_BEHAVIORS_SRCS)
target_sources(Lights PRIVATE ${LIGHTS_BEHAVIORS_SRCS})
target_include_directories(Lights PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Lights PRIVATE CK2 VxMath)

install(TARGETS Lights RUNTIME DESTINATION BuildingBlocks)
