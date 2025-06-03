# Controllers

add_library(Controllers SHARED Controllers.cpp Controllers.rc)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Behaviors CONTROLLERS_BEHAVIORS_SRCS)
target_sources(Controllers PRIVATE ${CONTROLLERS_BEHAVIORS_SRCS})
target_include_directories(Controllers PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Controllers PRIVATE CK2 VxMath)

install(TARGETS Controllers RUNTIME DESTINATION BuildingBlocks)