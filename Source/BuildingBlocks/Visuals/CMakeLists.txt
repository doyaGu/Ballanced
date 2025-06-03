# Visuals

add_library(Visuals SHARED Visuals.cpp Visuals.rc)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Behaviors VISUALS_BEHAVIORS_SRCS)
target_sources(Visuals PRIVATE ${VISUALS_BEHAVIORS_SRCS})
target_include_directories(Visuals PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Visuals PRIVATE CK2 VxMath)

install(TARGETS Visuals RUNTIME DESTINATION BuildingBlocks)