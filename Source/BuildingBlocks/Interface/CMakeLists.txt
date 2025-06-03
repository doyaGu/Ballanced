# Interface

set(INTERFACE_HEAHERS
        CKBitmapFont.h
        CKFontManager.h
        CKTextureFont.h
        CompiledData.h
        DrawingPrimitives.h
        Layout.h
        )

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Behaviors INTERFACE_BEHAVIORS_SOURCES)

set(INTERFACE_SOURCES
        CKBitmapFont.cpp
        CKFontManager.cpp
        CKTextureFont.cpp
        CompiledData.cpp
        DrawingPrimitives.cpp

        ${INTERFACE_BEHAVIORS_SOURCES}
        Interface.cpp
        Interface.rc
        )

add_library(Interface SHARED ${INTERFACE_SOURCES} ${INTERFACE_HEAHERS})
target_include_directories(Interface PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Interface PRIVATE CK2 VxMath)

install(TARGETS Interface RUNTIME DESTINATION BuildingBlocks)