# - Some useful macros and functions for creating building blocks

macro(add_building_block NAME)
	add_library(${NAME} SHARED)

	file(GLOB BB_${NAME}_HEADERS *.h)
	aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} BB_${NAME}_SOURCES)
	
	target_sources(${NAME} PRIVATE
		${BB_${NAME}_HEADERS}
		${BB_${NAME}_SOURCES}
		${NAME}.def
	)

	target_link_libraries(${NAME} PRIVATE
		VirtoolsSDK::CK2
		VirtoolsSDK::VxMath
	)
endmacro()

macro(install_building_block NAME)
	install(TARGETS ${NAME} RUNTIME DESTINATION ${BUILDING_BLOCKS_DIR})
endmacro()
