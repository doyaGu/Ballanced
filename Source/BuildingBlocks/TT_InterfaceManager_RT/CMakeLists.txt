# TT_InterfaceManager_RT

add_library(TT_InterfaceManager_RT SHARED TT_InterfaceManager_RT.cpp)
set(TT_INTERFACEMANAGER_RT_HEAHERS
        GameInfo.h
        NemoArray.h
        InterfaceManager.h
        )
set(TT_INTERFACEMANAGER_RT_SOURCES
        NemoArray.cpp
        InterfaceManager.cpp
        )
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Behaviors TT_INTERFACEMANAGER_RT_BEHAVIORS_SOURCES)
target_sources(TT_InterfaceManager_RT PRIVATE
    ${TT_INTERFACEMANAGER_RT_SOURCES}
    ${TT_INTERFACEMANAGER_RT_SOURCES}
    ${TT_INTERFACEMANAGER_RT_BEHAVIORS_SOURCES}
    )
target_include_directories(TT_InterfaceManager_RT PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(TT_InterfaceManager_RT PRIVATE CK2 VxMath)

install(TARGETS TT_InterfaceManager_RT RUNTIME DESTINATION BuildingBlocks)