# TT_Toolbox_RT

add_library(TT_Toolbox_RT SHARED TT_Toolbox_RT.cpp)
set(TT_TOOLBOX_RT_HEAHERS
        ToolboxGuids.h
        TimeManager.h
        DebugManager.h
        DXManager.h
        )
set(TT_TOOLBOX_RT_SOURCES
        TimeManager.cpp
        DebugManager.cpp
        DXManager.cpp
        )
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Behaviors TT_TOOLBOX_RT_BEHAVIORS_SOURCES)
target_sources(TT_Toolbox_RT PRIVATE
    ${TT_TOOLBOX_RT_SOURCES}
    ${TT_TOOLBOX_RT_HEAHERS}
    ${TT_TOOLBOX_RT_BEHAVIORS_SOURCES}
    )
target_include_directories(TT_Toolbox_RT PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(TT_Toolbox_RT PRIVATE CK2 VxMath)

install(TARGETS TT_Toolbox_RT RUNTIME DESTINATION BuildingBlocks)