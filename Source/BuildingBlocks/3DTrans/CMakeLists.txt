# 3DTrans

add_library(3DTrans SHARED 3DTrans.cpp PortalsManager.cpp PortalsManager.h 3DTrans.rc)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Behaviors 3DTRANS_BEHAVIORS_SRCS)
aux_source_directory(qhull QHULL_SRCS)
target_sources(3DTrans PRIVATE ${3DTRANS_BEHAVIORS_SRCS} ${QHULL_SRCS})
target_include_directories(3DTrans PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} qhull)
target_link_libraries(3DTrans PRIVATE CK2 VxMath)

copy_target_to_ballance_dir(3DTrans "BuildingBlocks")

install(TARGETS 3DTrans RUNTIME DESTINATION BuildingBlocks)