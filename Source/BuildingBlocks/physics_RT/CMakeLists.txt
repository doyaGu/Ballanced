# physics_RT

add_subdirectory(ivp)

add_library(physics_RT SHARED physics_RT.cpp)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Behaviors PHYSICS_RT_BEHAVIORS_SOURCES)
target_sources(physics_RT PRIVATE
        CKIpionManager.cpp
        CKIpionManager.h
        PhysicsCallback.cpp
        PhysicsCallback.h
        PhysicsContact.cpp
        PhysicsContact.h
        ${PHYSICS_RT_BEHAVIORS_SOURCES}
)
target_include_directories(physics_RT PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(physics_RT PRIVATE ivp_physics ivp_compact_builder CK2 VxMath)

install(TARGETS physics_RT RUNTIME DESTINATION BuildingBlocks)