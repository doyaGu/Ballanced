# Root-only compiler defaults for the superproject.
#
# Component projects own their language-standard requirements. Keeping the
# standard unset here prevents the superproject from silently changing the
# contract of a component that is also built standalone.

add_compile_definitions(
        $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
        $<$<C_COMPILER_ID:MSVC>:_CRT_NONSTDC_NO_WARNINGS>
        $<$<CONFIG:Debug>:DEBUG>
)

if (MSVC)
    add_compile_definitions(_MBCS)
    remove_definitions(-DUNICODE -D_UNICODE)

    set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD ON)

    if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W3" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else ()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
    endif ()

    add_compile_options(/MP)

    if (BALLANCE_BUILD_STATIC)
        ballance_set_cache_default(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" STRING
                "MSVC runtime library for static Player builds (default: /MT, /MTd)")
    else ()
        ballance_set_cache_default(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL" STRING
                "MSVC runtime library (default: /MD, /MDd)")
    endif ()
endif ()
