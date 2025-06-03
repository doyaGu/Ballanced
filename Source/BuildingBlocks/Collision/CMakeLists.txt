# Collision

add_library(Collision SHARED
        CollisionBehaviors.cpp
        CollisionManager.cpp
        CollisionManager.h
        CKCollisionManager_AdvancedIntersectionFunctions.cpp
        CKCollisionManager_Detection.cpp
        CKCollisionManager_ToolsFunctions.cpp
        BoundariesManager2.cpp
        BoundariesManager2.h
        FloorManager.cpp
        FloorManager.h
        FloorTools.cpp
        FloorTools.h
        N3dGraph.cpp
        N3dGraph.h
        CalcCollision.cpp
        CalcCollision.h
        CollisionBehaviors.rc)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Behaviors COLLISION_BEHAVIORS_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Managers COLLISION_MANAGERS_SRCS)
target_sources(Collision PRIVATE ${COLLISION_BEHAVIORS_SRCS} ${COLLISION_MANAGERS_SRCS})
target_include_directories(Collision PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Collision PRIVATE CK2 VxMath)

copy_target_to_ballance_dir(Collision "BuildingBlocks")

install(TARGETS Collision RUNTIME DESTINATION BuildingBlocks)