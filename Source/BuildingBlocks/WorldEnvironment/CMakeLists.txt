# WorldEnvironment

add_library(WorldEnvironment SHARED WorldEnvironment.cpp WorldEnvironment.rc)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Behaviors WORLDENVIRONMENT_BEHAVIORS_SRCS)
target_sources(WorldEnvironment PRIVATE ${WORLDENVIRONMENT_BEHAVIORS_SRCS})
target_include_directories(WorldEnvironment PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(WorldEnvironment PRIVATE CK2 VxMath)

install(TARGETS WorldEnvironment RUNTIME DESTINATION BuildingBlocks)