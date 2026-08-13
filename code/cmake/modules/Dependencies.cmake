include(${CMAKE_CURRENT_LIST_DIR}/Locale.cmake)

macro(ensure_package)

    set(options)
    set(oneValueArgs NAME REPO TAG INSTALL_PREFIX HEADER_SUBDIR)
    set(multiValueArgs TARGETS)

    cmake_parse_arguments(ENSURE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT ENSURE_NAME OR NOT ENSURE_REPO OR NOT ENSURE_TAG OR NOT ENSURE_INSTALL_PREFIX)
        st_l_error(EDependencies0)
    endif ()

    set(INSTALL_MARKER "${ENSURE_INSTALL_PREFIX}/.${ENSURE_NAME}_installed")

    if (EXISTS "${INSTALL_MARKER}")
        st_l_info_fmt(IDependencies1 ${ENSURE_NAME} ${ENSURE_INSTALL_PREFIX})

        set(${ENSURE_NAME}_READY TRUE PARENT_SCOPE)

        return()

    endif ()

    st_l_info_fmt(IDependencies2 ${ENSURE_NAME} ${ENSURE_REPO})

    include(FetchContent)
    FetchContent_Declare(${ENSURE_NAME}
            GIT_REPOSITORY "${ENSURE_REPO}"
            GIT_TAG "${ENSURE_TAG}"
            SOURCE_DIR "${ENSURE_INSTALL_PREFIX}/${ENSURE_NAME}-src"
            BINARY_DIR "${CMAKE_BINARY_DIR}/dependencies/${ENSURE_NAME}-build"
    )

    FetchContent_MakeAvailable(${ENSURE_NAME})

    if (ENSURE_TARGETS)
        install(TARGETS ${ENSURE_TARGETS}
                EXPORT ${ENSURE_NAME}Targets
                RUNTIME DESTINATION "${ENSURE_INSTALL_PREFIX}/bin"
                LIBRARY DESTINATION "${ENSURE_INSTALL_PREFIX}/lib"
                ARCHIVE DESTINATION "${ENSURE_INSTALL_PREFIX}/lib"
        )
    endif ()

    if (ENSURE_HEADER_SUBDIR)
        install(DIRECTORY "${${ENSURE_NAME}_SOURCE_DIR}/${ENSURE_HEADER_SUBDIR}"
                DESTINATION "${ENSURE_INSTALL_PREFIX}/include"
                FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
        )
    endif ()

    file(WRITE "${INSTALL_MARKER}" "")
    install(FILES "${INSTALL_MARKER}" DESTINATION "${ENSURE_INSTALL_PREFIX}")

    set(${ENSURE_NAME}_INSTALL_PREFIX ${ENSURE_INSTALL_PREFIX} CACHE INTERNAL "")

    set(${ENSURE_NAME}_READY TRUE PARENT_SCOPE)

endmacro()
