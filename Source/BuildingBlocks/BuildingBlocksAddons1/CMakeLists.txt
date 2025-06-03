# BuildingBlocksAddons1

add_library(BuildingBlocksAddons1 SHARED
        BuildingBlocksAddons1.cpp
        LightmapGenerator.cpp
        LightmapGenerator.h
        NearestPointGrid.cpp
        NearestPointGrid.h
        PolyhedronConstructor.cpp
        PolyhedronConstructor.h
        TextureProcessing.cpp
        TextureProcessing.h
        BuildingBlocksAddons.rc)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Behaviors BUILDINGBLOCKSADDONS1_BEHAVIORS_SRCS)
target_sources(BuildingBlocksAddons1 PRIVATE ${BUILDINGBLOCKSADDONS1_BEHAVIORS_SRCS})
target_include_directories(BuildingBlocksAddons1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(BuildingBlocksAddons1 PRIVATE CK2 VxMath)

copy_target_to_ballance_dir(BuildingBlocksAddons1 "BuildingBlocks")

install(TARGETS BuildingBlocksAddons1 RUNTIME DESTINATION BuildingBlocks)