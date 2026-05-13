set(BALLANCE_VERSION_TWEAK 0)
set(BALLANCE_RUNTIME_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${BALLANCE_VERSION_TWEAK}")
set(BALLANCE_VERSION_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}")

function(ballance_configure_version_resource OUT_VAR TARGET_NAME FILE_DESCRIPTION ORIGINAL_FILENAME)
    if (NOT DEFINED BALLANCE_MODULE_VERSION_OVERRIDE)
        message(FATAL_ERROR "BALLANCE_MODULE_VERSION_OVERRIDE is required")
    endif ()

    string(REPLACE "." ";" _version_parts "${BALLANCE_MODULE_VERSION_OVERRIDE}")
    list(LENGTH _version_parts _version_part_count)
    if (NOT _version_part_count EQUAL 4 OR NOT BALLANCE_MODULE_VERSION_OVERRIDE MATCHES "^[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+$")
        message(FATAL_ERROR "BALLANCE_MODULE_VERSION_OVERRIDE must be a numeric four-part version")
    endif ()

    list(GET _version_parts 0 _version_major)
    list(GET _version_parts 1 _version_minor)
    list(GET _version_parts 2 _version_patch)
    list(GET _version_parts 3 _version_tweak)

    set(BALLANCE_VERSION_MAJOR "${_version_major}")
    set(BALLANCE_VERSION_MINOR "${_version_minor}")
    set(BALLANCE_VERSION_PATCH "${_version_patch}")
    set(BALLANCE_VERSION_TWEAK "${_version_tweak}")
    set(BALLANCE_VERSION_STRING "${BALLANCE_MODULE_VERSION_OVERRIDE}")
    set(BALLANCE_VERSION_TARGET "${TARGET_NAME}")
    set(BALLANCE_VERSION_DESCRIPTION "${FILE_DESCRIPTION}")
    set(BALLANCE_VERSION_ORIGINAL_FILENAME "${ORIGINAL_FILENAME}")
    set(BALLANCE_VERSION_RESOURCE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}Version.generated.rc")

    configure_file(
            "${BALLANCE_VERSION_CMAKE_DIR}/BallanceVersionResource.rc.in"
            "${BALLANCE_VERSION_RESOURCE}"
            @ONLY
    )

    set(${OUT_VAR} "${BALLANCE_VERSION_RESOURCE}" PARENT_SCOPE)
endfunction()
