# Find the Virtools SDK header + import library
#
#  VIRTOOLS_SDK_INCLUDE_DIR - Where to find CKAll.h
#  VIRTOOLS_SDK_LIBRARIES   - List of libraries when using VirtoolsSDK.
#  VIRTOOLS_SDK_FOUND       - True if Virtools SDK found.
#  VirtoolsSDK::CK2         - Imported library of CK2
#  VirtoolsSDK::VxMath      - Imported library of VxMath

set(VIRTOOLS_SDK_SEARCH_PATHS
	"${PROJECT_SOURCE_DIR}/SDK"
	"${VIRTOOLS_SDK_PATH}"
)

find_path(VIRTOOLS_SDK_INCLUDE_DIR CKAll.h
	HINTS $ENV{VIRTOOLS_SDK}
    PATHS ${VIRTOOLS_SDK_SEARCH_PATHS}
    PATH_SUFFIXES Include
)

set(VIRTOOLS_SDK_LIBNAMES
	CK2
	VxMath
)

set(VIRTOOLS_SDK_LIBRARIES)
foreach(LIBNAME IN LISTS VIRTOOLS_SDK_LIBNAMES)
	find_library(VIRTOOLS_SDK_${LIBNAME} NAMES ${LIBNAME}
		HINTS $ENV{VIRTOOLS_SDK}
		PATHS ${VIRTOOLS_SDK_SEARCH_PATHS}
		PATH_SUFFIXES Lib
	)
	list(APPEND VIRTOOLS_SDK_LIBRARIES ${VIRTOOLS_SDK_${LIBNAME}})
endforeach()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(VirtoolsSDK DEFAULT_MSG VIRTOOLS_SDK_LIBRARIES VIRTOOLS_SDK_INCLUDE_DIR)

foreach(LIBNAME IN LISTS VIRTOOLS_SDK_LIBNAMES)
	if(NOT TARGET ${LIBNAME})
		add_library(${LIBNAME} UNKNOWN IMPORTED)
		add_library(VirtoolsSDK::${LIBNAME} ALIAS ${LIBNAME})
		set_target_properties(${LIBNAME} PROPERTIES
			IMPORTED_LOCATION ${VIRTOOLS_SDK_${LIBNAME}}
			INTERFACE_INCLUDE_DIRECTORIES ${VIRTOOLS_SDK_INCLUDE_DIR}
		)
		# Disable strict const-qualification conformance for pointers initialized by using string literals
		target_compile_options(${LIBNAME} INTERFACE
			$<$<COMPILE_LANGUAGE:CXX>:$<$<CXX_COMPILER_ID:MSVC>:/Zc:strictStrings->>
			$<$<COMPILE_LANGUAGE:CXX>:$<$<CXX_COMPILER_ID:GNU>:-fpermissive>>
			$<$<COMPILE_LANGUAGE:CXX>:$<$<CXX_COMPILER_ID:GNU>:-Wno-write-strings>>
		)
	endif()
endforeach()
