# TT_DatabaseManager_RT

add_library(TT_DatabaseManager_RT SHARED TT_DatabaseManager_RT.cpp)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Behaviors TT_DATABASEMANAGER_RT_BEHAVIORS_SOURCES)
target_sources(TT_DatabaseManager_RT PRIVATE
    DatabaseManager.cpp
    DatabaseManager.h
    ${TT_DATABASEMANAGER_RT_BEHAVIORS_SOURCES}
    )
target_include_directories(TT_DatabaseManager_RT PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(TT_DatabaseManager_RT PRIVATE CK2 VxMath)

install(TARGETS TT_DatabaseManager_RT RUNTIME DESTINATION BuildingBlocks)