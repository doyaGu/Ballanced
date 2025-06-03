# Materials

add_library(Materials SHARED Materials.cpp Materials.rc)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Behaviors MATERIALS_BEHAVIORS_SRCS)
target_sources(Materials PRIVATE ${MATERIALS_BEHAVIORS_SRCS})
target_include_directories(Materials PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Materials PRIVATE CK2 VxMath)

install(TARGETS Materials RUNTIME DESTINATION BuildingBlocks)