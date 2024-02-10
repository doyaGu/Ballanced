# physics_RT

# Ipion Virtual Physics
set(IVP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ivp)

set(IVP_COLLISION_DIR ${IVP_DIR}/ivp_collision)

set(IVP_COLLISION_SOURCES
        ${IVP_COLLISION_DIR}/ivp_3d_solver.cxx
        ${IVP_COLLISION_DIR}/ivp_3d_solver.hxx
        ${IVP_COLLISION_DIR}/ivp_cache_ledge_point.hxx
        ${IVP_COLLISION_DIR}/ivp_clustering_longrange.cxx
        ${IVP_COLLISION_DIR}/ivp_clustering_longrange.hxx
        ${IVP_COLLISION_DIR}/ivp_clustering_lrange_hash.cxx
        ${IVP_COLLISION_DIR}/ivp_clustering_lrange_hash.hxx
        ${IVP_COLLISION_DIR}/ivp_clustering_visual_hash.cxx
        ${IVP_COLLISION_DIR}/ivp_clustering_visual_hash.hxx
        ${IVP_COLLISION_DIR}/ivp_clustering_visualizer.cxx
        ${IVP_COLLISION_DIR}/ivp_clustering_visualizer.hxx
        ${IVP_COLLISION_DIR}/ivp_coll_del_root_mindist.cxx
        ${IVP_COLLISION_DIR}/ivp_collision.hxx
        ${IVP_COLLISION_DIR}/ivp_collision_filter.cxx
        ${IVP_COLLISION_DIR}/ivp_collision_filter.hxx
        ${IVP_COLLISION_DIR}/ivp_compact_ledge.cxx
        ${IVP_COLLISION_DIR}/ivp_compact_ledge.hxx
        ${IVP_COLLISION_DIR}/ivp_compact_ledge_solver.cxx
        ${IVP_COLLISION_DIR}/ivp_compact_ledge_solver.hxx
        ${IVP_COLLISION_DIR}/ivp_i_collision_vhash.cxx
        ${IVP_COLLISION_DIR}/ivp_i_collision_vhash.hxx
        ${IVP_COLLISION_DIR}/ivp_listener_collision.hxx
        ${IVP_COLLISION_DIR}/ivp_mindist.cxx
        ${IVP_COLLISION_DIR}/ivp_mindist.hxx
        ${IVP_COLLISION_DIR}/ivp_mindist_debug.cxx
        ${IVP_COLLISION_DIR}/ivp_mindist_event.cxx
        ${IVP_COLLISION_DIR}/ivp_mindist_event.hxx
        ${IVP_COLLISION_DIR}/ivp_mindist_intern.hxx
        ${IVP_COLLISION_DIR}/ivp_mindist_macros.hxx
        ${IVP_COLLISION_DIR}/ivp_mindist_mcases.cxx
        ${IVP_COLLISION_DIR}/ivp_mindist_minimize.cxx
        ${IVP_COLLISION_DIR}/ivp_mindist_minimize.hxx
        ${IVP_COLLISION_DIR}/ivp_mindist_recursive.cxx
        ${IVP_COLLISION_DIR}/ivp_oo_watcher.cxx
        ${IVP_COLLISION_DIR}/ivp_range_manager.cxx
        ${IVP_COLLISION_DIR}/ivp_range_manager.hxx
        ${IVP_COLLISION_DIR}/ivp_ray_solver.cxx
        ${IVP_COLLISION_DIR}/ivp_ray_solver.hxx
        ${IVP_COLLISION_DIR}/ivp_universe_manager.hxx
)

set(IVP_CONTROLLER_DIR ${IVP_DIR}/ivp_controller)

set(IVP_CONTROLLER_SOURCES
        ${IVP_CONTROLLER_DIR}/ivp_actuator.cxx
        ${IVP_CONTROLLER_DIR}/ivp_actuator.hxx
        ${IVP_CONTROLLER_DIR}/ivp_actuator_info.hxx
        ${IVP_CONTROLLER_DIR}/ivp_actuator_spring.cxx
        ${IVP_CONTROLLER_DIR}/ivp_actuator_spring.hxx
        ${IVP_CONTROLLER_DIR}/ivp_attacher_to_cores.hxx
        ${IVP_CONTROLLER_DIR}/ivp_buoyancy_solver.cxx
        ${IVP_CONTROLLER_DIR}/ivp_buoyancy_solver.hxx
        ${IVP_CONTROLLER_DIR}/ivp_car_system.cxx
        ${IVP_CONTROLLER_DIR}/ivp_car_system.hxx
        ${IVP_CONTROLLER_DIR}/ivp_constraint.cxx
        ${IVP_CONTROLLER_DIR}/ivp_constraint.hxx
        ${IVP_CONTROLLER_DIR}/ivp_constraint_car.cxx
        ${IVP_CONTROLLER_DIR}/ivp_constraint_car.hxx
        ${IVP_CONTROLLER_DIR}/ivp_constraint_fixed_keyed.cxx
        ${IVP_CONTROLLER_DIR}/ivp_constraint_fixed_keyed.hxx
        ${IVP_CONTROLLER_DIR}/ivp_constraint_local.cxx
        ${IVP_CONTROLLER_DIR}/ivp_constraint_local.hxx
        ${IVP_CONTROLLER_DIR}/ivp_constraint_types.hxx
        ${IVP_CONTROLLER_DIR}/ivp_controller.hxx
        ${IVP_CONTROLLER_DIR}/ivp_controller_airboat.cpp
        ${IVP_CONTROLLER_DIR}/ivp_controller_airboat.h
        ${IVP_CONTROLLER_DIR}/ivp_controller_buoyancy.cxx
        ${IVP_CONTROLLER_DIR}/ivp_controller_buoyancy.hxx
        ${IVP_CONTROLLER_DIR}/ivp_controller_floating.cxx
        ${IVP_CONTROLLER_DIR}/ivp_controller_floating.hxx
        ${IVP_CONTROLLER_DIR}/ivp_controller_golem.hxx
        ${IVP_CONTROLLER_DIR}/ivp_controller_motion.cxx
        ${IVP_CONTROLLER_DIR}/ivp_controller_motion.hxx
        ${IVP_CONTROLLER_DIR}/ivp_controller_raycast_car.cxx
        ${IVP_CONTROLLER_DIR}/ivp_controller_raycast_car.hxx
        ${IVP_CONTROLLER_DIR}/ivp_controller_stiff_spring.cxx
        ${IVP_CONTROLLER_DIR}/ivp_controller_stiff_spring.hxx
        ${IVP_CONTROLLER_DIR}/ivp_controller_world_frict.cxx
        ${IVP_CONTROLLER_DIR}/ivp_controller_world_frict.hxx
        ${IVP_CONTROLLER_DIR}/ivp_forcefield.cxx
        ${IVP_CONTROLLER_DIR}/ivp_forcefield.hxx
        ${IVP_CONTROLLER_DIR}/ivp_multidimensional_interp.cxx
        ${IVP_CONTROLLER_DIR}/ivp_multidimensional_interp.hxx
        ${IVP_CONTROLLER_DIR}/ivp_template_constraint.cxx
        ${IVP_CONTROLLER_DIR}/ivp_template_constraint.hxx
)

set(IVP_INTERN_DIR ${IVP_DIR}/ivp_intern)

set(IVP_INTERN_SOURCES
        ${IVP_INTERN_DIR}/ivp_ball.cxx
        ${IVP_INTERN_DIR}/ivp_calc_next_psi_solver.cxx
        ${IVP_INTERN_DIR}/ivp_calc_next_psi_solver.hxx
        ${IVP_INTERN_DIR}/ivp_controller_phantom.cxx
        ${IVP_INTERN_DIR}/ivp_core.cxx
        ${IVP_INTERN_DIR}/ivp_environment.cxx
        ${IVP_INTERN_DIR}/ivp_friction.cxx
        ${IVP_INTERN_DIR}/ivp_friction.hxx
        ${IVP_INTERN_DIR}/ivp_friction_gaps.cxx
        ${IVP_INTERN_DIR}/ivp_friction_solver.hxx
        ${IVP_INTERN_DIR}/ivp_great_matrix.cxx
        ${IVP_INTERN_DIR}/ivp_hull_manager.cxx
        ${IVP_INTERN_DIR}/ivp_hull_manager_macros.hxx
        ${IVP_INTERN_DIR}/ivp_i_controller_vhash.hxx
        ${IVP_INTERN_DIR}/ivp_i_friction_hash.cxx
        ${IVP_INTERN_DIR}/ivp_i_friction_hash.hxx
        ${IVP_INTERN_DIR}/ivp_i_object_vhash.cxx
        ${IVP_INTERN_DIR}/ivp_i_object_vhash.hxx
        ${IVP_INTERN_DIR}/ivp_impact.cxx
        ${IVP_INTERN_DIR}/ivp_impact.hxx
        ${IVP_INTERN_DIR}/ivp_merge_core.cxx
        ${IVP_INTERN_DIR}/ivp_merge_core.hxx
        ${IVP_INTERN_DIR}/ivp_mindist_friction.cxx
        ${IVP_INTERN_DIR}/ivp_object.cxx
        ${IVP_INTERN_DIR}/ivp_object_attach.cxx
        ${IVP_INTERN_DIR}/ivp_physic.cxx
        ${IVP_INTERN_DIR}/ivp_physic_private.cxx
        ${IVP_INTERN_DIR}/ivp_physic_private.hxx
        ${IVP_INTERN_DIR}/ivp_polygon.cxx
        ${IVP_INTERN_DIR}/ivp_sim_unit.cxx
        ${IVP_INTERN_DIR}/ivp_sim_unit.hxx
        ${IVP_INTERN_DIR}/ivp_solver_core_reaction.cxx
        ${IVP_INTERN_DIR}/ivp_time.cxx
)

set(IVP_PHYSICS_DIR ${IVP_DIR}/ivp_physics)

set(IVP_PHYSICS_SOURCES
        ${IVP_PHYSICS_DIR}/ive_graphics.hxx
        ${IVP_PHYSICS_DIR}/ivp_anomaly_manager.cxx
        ${IVP_PHYSICS_DIR}/ivp_anomaly_manager.hxx
        ${IVP_PHYSICS_DIR}/ivp_authenticity.hxx
        ${IVP_PHYSICS_DIR}/ivp_ball.hxx
        ${IVP_PHYSICS_DIR}/ivp_betterdebugmanager.cxx
        ${IVP_PHYSICS_DIR}/ivp_betterdebugmanager.hxx
        ${IVP_PHYSICS_DIR}/ivp_betterstatisticsmanager.cxx
        ${IVP_PHYSICS_DIR}/ivp_betterstatisticsmanager.hxx
        ${IVP_PHYSICS_DIR}/ivp_cache_object.cxx
        ${IVP_PHYSICS_DIR}/ivp_cache_object.hxx
        ${IVP_PHYSICS_DIR}/ivp_contact_situation.hxx
        ${IVP_PHYSICS_DIR}/ivp_core.hxx
        ${IVP_PHYSICS_DIR}/ivp_core_macros.hxx
        ${IVP_PHYSICS_DIR}/ivp_debug.hxx
        ${IVP_PHYSICS_DIR}/ivp_debug_manager.hxx
        ${IVP_PHYSICS_DIR}/ivp_environment.hxx
        ${IVP_PHYSICS_DIR}/ivp_great_matrix.hxx
        ${IVP_PHYSICS_DIR}/ivp_hull_manager.hxx
        ${IVP_PHYSICS_DIR}/ivp_liquid_surface_descript.cxx
        ${IVP_PHYSICS_DIR}/ivp_liquid_surface_descript.hxx
        ${IVP_PHYSICS_DIR}/ivp_listener_hull.hxx
        ${IVP_PHYSICS_DIR}/ivp_listener_object.hxx
        ${IVP_PHYSICS_DIR}/ivp_listener_psi.hxx
        ${IVP_PHYSICS_DIR}/ivp_material.cxx
        ${IVP_PHYSICS_DIR}/ivp_material.hxx
        ${IVP_PHYSICS_DIR}/ivp_object.hxx
        ${IVP_PHYSICS_DIR}/ivp_object_attach.hxx
        ${IVP_PHYSICS_DIR}/ivp_performancecounter.cxx
        ${IVP_PHYSICS_DIR}/ivp_performancecounter.hxx
        ${IVP_PHYSICS_DIR}/ivp_phantom.hxx
        ${IVP_PHYSICS_DIR}/ivp_physics.hxx
        ${IVP_PHYSICS_DIR}/ivp_polygon.hxx
        ${IVP_PHYSICS_DIR}/ivp_radar.hxx
        ${IVP_PHYSICS_DIR}/ivp_radar_appl.hxx
        ${IVP_PHYSICS_DIR}/ivp_real_object.hxx
        ${IVP_PHYSICS_DIR}/ivp_solver_core_reaction.hxx
        ${IVP_PHYSICS_DIR}/ivp_stat_manager_cback_con.cxx
        ${IVP_PHYSICS_DIR}/ivp_stat_manager_cback_con.hxx
        ${IVP_PHYSICS_DIR}/ivp_surface_manager.cxx
        ${IVP_PHYSICS_DIR}/ivp_surface_manager.hxx
        ${IVP_PHYSICS_DIR}/ivp_templates.cxx
        ${IVP_PHYSICS_DIR}/ivp_templates.hxx
        ${IVP_PHYSICS_DIR}/ivp_time.hxx
        ${IVP_PHYSICS_DIR}/ivp_time_event.hxx
)

set(IVP_SURFACEMANAGER_DIR ${IVP_DIR}/ivp_surface_manager)

set(IVP_SURFACEMANAGER_SOURCES
        ${IVP_SURFACEMANAGER_DIR}/ivp_compact_grid.hxx
        ${IVP_SURFACEMANAGER_DIR}/ivp_compact_surface.cxx
        ${IVP_SURFACEMANAGER_DIR}/ivp_compact_surface.hxx
        ${IVP_SURFACEMANAGER_DIR}/ivp_gridbuild_array.cxx
        ${IVP_SURFACEMANAGER_DIR}/ivp_gridbuild_array.hxx
        ${IVP_SURFACEMANAGER_DIR}/ivp_surman_grid.cxx
        ${IVP_SURFACEMANAGER_DIR}/ivp_surman_grid.hxx
        ${IVP_SURFACEMANAGER_DIR}/ivp_surman_polygon.cxx
        ${IVP_SURFACEMANAGER_DIR}/ivp_surman_polygon.hxx
)

set(IVP_UTILITY_DIR ${IVP_DIR}/ivp_utility)

set(IVP_UTILITY_SOURCES
        ${IVP_UTILITY_DIR}/ivu_active_value.cxx
        ${IVP_UTILITY_DIR}/ivu_active_value.hxx
        ${IVP_UTILITY_DIR}/ivu_active_value_hash.hxx
        ${IVP_UTILITY_DIR}/ivu_bigvector.cxx
        ${IVP_UTILITY_DIR}/ivu_bigvector.hxx
        ${IVP_UTILITY_DIR}/ivu_diff_hash.hxx
        ${IVP_UTILITY_DIR}/ivu_float.hxx
        ${IVP_UTILITY_DIR}/ivu_fvector.hxx
        ${IVP_UTILITY_DIR}/ivu_geometry.cxx
        ${IVP_UTILITY_DIR}/ivu_geometry.hxx
        ${IVP_UTILITY_DIR}/ivu_hash.cxx
        ${IVP_UTILITY_DIR}/ivu_hash.hxx
        ${IVP_UTILITY_DIR}/ivu_linear.cxx
        ${IVP_UTILITY_DIR}/ivu_linear.hxx
        ${IVP_UTILITY_DIR}/ivu_linear_double.hxx
        ${IVP_UTILITY_DIR}/ivu_linear_macros.hxx
        ${IVP_UTILITY_DIR}/ivu_linear_PIII.hxx
        ${IVP_UTILITY_DIR}/ivu_linear_ps2.hxx
        ${IVP_UTILITY_DIR}/ivu_linear_software.hxx
        ${IVP_UTILITY_DIR}/ivu_linear_willamette.hxx
        ${IVP_UTILITY_DIR}/ivu_list.hxx
        ${IVP_UTILITY_DIR}/ivu_mapping.hxx
        ${IVP_UTILITY_DIR}/ivu_matrix_macros.hxx
        ${IVP_UTILITY_DIR}/ivu_memory.cxx
        ${IVP_UTILITY_DIR}/ivu_memory.hxx
        ${IVP_UTILITY_DIR}/ivu_min_hash.cxx
        ${IVP_UTILITY_DIR}/ivu_min_hash.hxx
        ${IVP_UTILITY_DIR}/ivu_min_list.cxx
        ${IVP_UTILITY_DIR}/ivu_min_list.hxx
        ${IVP_UTILITY_DIR}/ivu_os_dep.cxx
        ${IVP_UTILITY_DIR}/ivu_ps2.hxx
        ${IVP_UTILITY_DIR}/ivu_quat.cxx
        ${IVP_UTILITY_DIR}/ivu_quat.hxx
        ${IVP_UTILITY_DIR}/ivu_set.hxx
        ${IVP_UTILITY_DIR}/ivu_string.cxx
        ${IVP_UTILITY_DIR}/ivu_string.hxx
        ${IVP_UTILITY_DIR}/ivu_string_hash.hxx
        ${IVP_UTILITY_DIR}/ivu_types.cxx
        ${IVP_UTILITY_DIR}/ivu_types.hxx
        ${IVP_UTILITY_DIR}/ivu_vector.cxx
        ${IVP_UTILITY_DIR}/ivu_vector.hxx
        ${IVP_UTILITY_DIR}/ivu_vhash.cxx
        ${IVP_UTILITY_DIR}/ivu_vhash.hxx
)

add_library(ivp_physics STATIC)
target_sources(ivp_physics PRIVATE
        ${IVP_COLLISION_SOURCES}
        ${IVP_CONTROLLER_SOURCES}
        ${IVP_INTERN_SOURCES}
        ${IVP_PHYSICS_SOURCES}
        ${IVP_SURFACEMANAGER_SOURCES}
        ${IVP_UTILITY_SOURCES}
)
target_include_directories(ivp_physics PUBLIC
        ${IVP_COLLISION_DIR}
        ${IVP_CONTROLLER_DIR}
        ${IVP_INTERN_DIR}
        ${IVP_PHYSICS_DIR}
        ${IVP_SURFACEMANAGER_DIR}
        ${IVP_UTILITY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/havana
        ${CMAKE_CURRENT_SOURCE_DIR}/havana/havok
)
target_compile_definitions(ivp_physics PRIVATE
        IVP_VERSION_SDK
)

set(IVP_COMPACT_BUILDER_DIR ${IVP_DIR}/ivp_compact_builder)

set(IVP_COMPACT_BUILDER_SOURCES
        ${IVP_COMPACT_BUILDER_DIR}/ivp_compact_ledge_gen.cxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_compact_ledge_gen.hxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_compact_modify.cxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_compact_modify.hxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_compact_recursive.cxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_compact_recursive.hxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_convex_decompositor.cxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_convex_decompositor.hxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_halfspacesoup.cxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_halfspacesoup.hxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_i_fpoint_vhash.cxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_i_fpoint_vhash.hxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_i_point_vhash.cxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_i_point_vhash.hxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_object_polygon_tetra.cxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_object_polygon_tetra.hxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_rot_inertia_solver.cxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_rot_inertia_solver.hxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_surbuild_3ds.hxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_surbuild_halfspacesoup.cxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_surbuild_halfspacesoup.hxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_surbuild_ledge_soup.cxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_surbuild_ledge_soup.hxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_surbuild_pointsoup.cxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_surbuild_pointsoup.hxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_surbuild_polygon_convex.cxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_surbuild_polygon_convex.hxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_surbuild_polyhdrn_cncv.cxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_surbuild_polyhdrn_cncv.hxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_surbuild_q12.cxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_surbuild_q12.hxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_templates_intern.cxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_templates_intern.hxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_tetra_intrude.cxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_tetra_intrude.hxx
        ${IVP_COMPACT_BUILDER_DIR}/ivp_triangle_gen.hxx
        ${IVP_COMPACT_BUILDER_DIR}/ivv_cluster_min_hash.cxx
        ${IVP_COMPACT_BUILDER_DIR}/ivv_cluster_min_hash.hxx
)

set(IVP_QHULL_SOURCES
        ${IVP_COMPACT_BUILDER_DIR}/qhull.cxx
        ${IVP_COMPACT_BUILDER_DIR}/qhull.hxx
        ${IVP_COMPACT_BUILDER_DIR}/qhull_a.hxx
        ${IVP_COMPACT_BUILDER_DIR}/qhull_geom.cxx
        ${IVP_COMPACT_BUILDER_DIR}/qhull_geom.hxx
        ${IVP_COMPACT_BUILDER_DIR}/qhull_geom2.cxx
        ${IVP_COMPACT_BUILDER_DIR}/qhull_global.cxx
        ${IVP_COMPACT_BUILDER_DIR}/qhull_io.cxx
        ${IVP_COMPACT_BUILDER_DIR}/qhull_io.hxx
        ${IVP_COMPACT_BUILDER_DIR}/qhull_mem.cxx
        ${IVP_COMPACT_BUILDER_DIR}/qhull_mem.hxx
        ${IVP_COMPACT_BUILDER_DIR}/qhull_merge.cxx
        ${IVP_COMPACT_BUILDER_DIR}/qhull_merge.hxx
        ${IVP_COMPACT_BUILDER_DIR}/qhull_poly.cxx
        ${IVP_COMPACT_BUILDER_DIR}/qhull_poly.hxx
        ${IVP_COMPACT_BUILDER_DIR}/qhull_poly2.cxx
        ${IVP_COMPACT_BUILDER_DIR}/qhull_qset.cxx
        ${IVP_COMPACT_BUILDER_DIR}/qhull_qset.hxx
        ${IVP_COMPACT_BUILDER_DIR}/qhull_stat.cxx
        ${IVP_COMPACT_BUILDER_DIR}/qhull_stat.hxx
        ${IVP_COMPACT_BUILDER_DIR}/qhull_user.cxx
        ${IVP_COMPACT_BUILDER_DIR}/qhull_user.hxx
)

set(IVP_GEOMPACK_SOURCES
        ${IVP_COMPACT_BUILDER_DIR}/geompack.hxx
        ${IVP_COMPACT_BUILDER_DIR}/geompack_cutfac.cxx
        ${IVP_COMPACT_BUILDER_DIR}/geompack_cvdec3.cxx
        ${IVP_COMPACT_BUILDER_DIR}/geompack_drdec3.cxx
        ${IVP_COMPACT_BUILDER_DIR}/geompack_dsphdc.cxx
        ${IVP_COMPACT_BUILDER_DIR}/geompack_edght.cxx
        ${IVP_COMPACT_BUILDER_DIR}/geompack_initcb.cxx
        ${IVP_COMPACT_BUILDER_DIR}/geompack_insed3.cxx
        ${IVP_COMPACT_BUILDER_DIR}/geompack_insfac.cxx
        ${IVP_COMPACT_BUILDER_DIR}/geompack_insvr3.cxx
        ${IVP_COMPACT_BUILDER_DIR}/geompack_prime.cxx
        ${IVP_COMPACT_BUILDER_DIR}/geompack_ptpolg.cxx
        ${IVP_COMPACT_BUILDER_DIR}/geompack_resedg.cxx
)

set(IVP_3D_STUDIO_OBJECT_READER_SOURCES
        ${IVP_COMPACT_BUILDER_DIR}/3dsimport_co.cxx
        ${IVP_COMPACT_BUILDER_DIR}/3dsimport_load.hxx
)

add_library(ivp_compact_builder STATIC)
target_sources(ivp_compact_builder PRIVATE
        ${IVP_COMPACT_BUILDER_SOURCES}
        ${IVP_QHULL_SOURCES}
        ${IVP_GEOMPACK_SOURCES}
        ${IVP_3D_STUDIO_OBJECT_READER_SOURCES}
)
target_include_directories(ivp_compact_builder PUBLIC
        ${IVP_COMPACT_BUILDER_DIR}
        ${IVP_COLLISION_DIR}
        ${IVP_CONTROLLER_DIR}
        ${IVP_INTERN_DIR}
        ${IVP_PHYSICS_DIR}
        ${IVP_SURFACEMANAGER_DIR}
        ${IVP_UTILITY_DIR}
)
target_compile_definitions(ivp_compact_builder PRIVATE IVP_VERSION_SDK)

add_library(physics_RT SHARED physics_RT.cpp)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Behaviors PHYSICS_RT_BEHAVIORS_SOURCES)
target_sources(physics_RT PRIVATE
        CKIpionManager.cpp
        CKIpionManager.h
        ${PHYSICS_RT_BEHAVIORS_SOURCES}
)
target_include_directories(physics_RT PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(physics_RT PRIVATE ivp_physics ivp_compact_builder CK2 VxMath)