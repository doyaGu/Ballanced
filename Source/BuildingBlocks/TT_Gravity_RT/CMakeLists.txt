# TT_Gravity_RT

add_library(TT_Gravity_RT SHARED TT_Gravity_RT.cpp)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Behaviors TT_GRAVITY_RT_BEHAVIORS_SOURCES)
target_sources(TT_Gravity_RT PRIVATE ${TT_GRAVITY_RT_BEHAVIORS_SOURCES})
target_include_directories(TT_Gravity_RT PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(TT_Gravity_RT PRIVATE CK2 VxMath)

install(TARGETS TT_Gravity_RT RUNTIME DESTINATION BuildingBlocks)