# The engine must publish its targets before Player selects link dependencies.
add_subdirectory(NeMo2)
add_subdirectory(Player)

if (MSVC)
    set_target_properties(Player PROPERTIES FOLDER "Applications")
endif ()
message(STATUS "[Ballance] Install outputs: ${CMAKE_INSTALL_PREFIX}/")
