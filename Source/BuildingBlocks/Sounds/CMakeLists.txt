# Sounds

add_library(Sounds SHARED SoundBehavior.cpp ACD.cpp ACD.h SoundBehavior.cpp)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Behaviors SOUNDS_BEHAVIORS_SRCS)
target_sources(Sounds PRIVATE ${SOUNDS_BEHAVIORS_SRCS})
target_include_directories(Sounds PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Sounds PRIVATE CK2 VxMath winmm)

install(TARGETS Sounds RUNTIME DESTINATION BuildingBlocks)