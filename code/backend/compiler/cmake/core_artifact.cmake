if(IC10_CORE_ARTIFACT_DIR AND EXISTS "${IC10_CORE_ARTIFACT_DIR}")
    message(STATUS "Using prebuilt IC10 core artifact from: ${IC10_CORE_ARTIFACT_DIR}")

    set(CORE_ARTIFACT_INCLUDE_DIR "${IC10_CORE_ARTIFACT_DIR}/include")
    set(CORE_ARTIFACT_LIB_DIR "${IC10_CORE_ARTIFACT_DIR}/lib")

    add_library(common STATIC IMPORTED)
    add_library(ic10_lib STATIC IMPORTED)

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CORE_BUILD_TYPE "Debug")
    else()
        set(CORE_BUILD_TYPE "Release")
    endif()

    if(MSVC)
        set(COMMON_LIB_PATH "${CORE_ARTIFACT_LIB_DIR}/common/${CORE_BUILD_TYPE}/common.lib")
        set(IC10_LIB_PATH "${CORE_ARTIFACT_LIB_DIR}/IC10/${CORE_BUILD_TYPE}/ic10_lib.lib")
    else()
        # Linux/Ninja: libraries are in lib/common/Release/ subdirectory
        set(COMMON_LIB_PATH "${CORE_ARTIFACT_LIB_DIR}/common/${CORE_BUILD_TYPE}/libcommon.a")
        set(IC10_LIB_PATH "${CORE_ARTIFACT_LIB_DIR}/IC10/${CORE_BUILD_TYPE}/libic10_lib.a")
    endif()

    if(NOT EXISTS "${COMMON_LIB_PATH}")
        message(FATAL_ERROR "common library not found in artifact: ${COMMON_LIB_PATH}")
    endif()

    if(NOT EXISTS "${IC10_LIB_PATH}")
        message(FATAL_ERROR "ic10_lib library not found in artifact: ${IC10_LIB_PATH}")
    endif()

    set_target_properties(common PROPERTIES
        IMPORTED_LOCATION "${COMMON_LIB_PATH}"
        INTERFACE_INCLUDE_DIRECTORIES "${CORE_ARTIFACT_INCLUDE_DIR}/common"
    )

    set_target_properties(ic10_lib PROPERTIES
        IMPORTED_LOCATION "${IC10_LIB_PATH}"
        INTERFACE_INCLUDE_DIRECTORIES "${CORE_ARTIFACT_INCLUDE_DIR}/IC10;${CORE_ARTIFACT_INCLUDE_DIR}/common"
    )

    message(STATUS "Imported common library: ${COMMON_LIB_PATH}")
    message(STATUS "Imported ic10_lib library: ${IC10_LIB_PATH}")

    set(COMMON_INCLUDE_DIR "${CORE_ARTIFACT_INCLUDE_DIR}/common" CACHE PATH "Common include directory" FORCE)

else()
    message(STATUS "Building IC10 core from source")
endif()