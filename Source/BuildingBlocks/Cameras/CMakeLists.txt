# Cameras

add_library(Cameras SHARED Cameras.cpp Cameras.rc)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Behaviors CAMERAS_BEHAVIORS_SRCS)
target_sources(Cameras PRIVATE ${CAMERAS_BEHAVIORS_SRCS})
target_include_directories(Cameras PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Cameras PRIVATE CK2 VxMath)

copy_target_to_ballance_dir(Cameras "BuildingBlocks")

install(TARGETS Cameras RUNTIME DESTINATION BuildingBlocks)