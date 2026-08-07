if(IC10_RUNTIME_CORE_ARTIFACT_DIR AND EXISTS "${IC10_RUNTIME_CORE_ARTIFACT_DIR}")
    message(STATUS "Using prebuilt IC10 runtime core artifact from: ${IC10_RUNTIME_CORE_ARTIFACT_DIR}")

    set(CORE_ARTIFACT_INCLUDE_DIR "${IC10_RUNTIME_CORE_ARTIFACT_DIR}/include")
    set(CORE_ARTIFACT_LIB_DIR "${IC10_RUNTIME_CORE_ARTIFACT_DIR}/lib")

    add_library(common STATIC IMPORTED)
    add_library(ic10_runtime_lib STATIC IMPORTED)

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CORE_BUILD_TYPE "Debug")
    else()
        set(CORE_BUILD_TYPE "Release")
    endif()

    if(MSVC)
        set(COMMON_LIB_PATH "${CORE_ARTIFACT_LIB_DIR}/common/${CORE_BUILD_TYPE}/common.lib")
        set(RUNTIME_LIB_PATH "${CORE_ARTIFACT_LIB_DIR}/core/${CORE_BUILD_TYPE}/ic10_runtime_lib.lib")
    else()
        set(COMMON_LIB_PATH "${CORE_ARTIFACT_LIB_DIR}/common/${CORE_BUILD_TYPE}/libcommon.a")
        set(RUNTIME_LIB_PATH "${CORE_ARTIFACT_LIB_DIR}/core/${CORE_BUILD_TYPE}/libic10_runtime_lib.a")
    endif()

    if(NOT EXISTS "${COMMON_LIB_PATH}")
        message(FATAL_ERROR "common library not found in artifact: ${COMMON_LIB_PATH}")
    endif()

    if(NOT EXISTS "${RUNTIME_LIB_PATH}")
        message(FATAL_ERROR "ic10_runtime_lib library not found in artifact: ${RUNTIME_LIB_PATH}")
    endif()

    set_target_properties(common PROPERTIES
        IMPORTED_LOCATION "${COMMON_LIB_PATH}"
        INTERFACE_INCLUDE_DIRECTORIES "${CORE_ARTIFACT_INCLUDE_DIR}/common"
    )

    set_target_properties(ic10_runtime_lib PROPERTIES
        IMPORTED_LOCATION "${RUNTIME_LIB_PATH}"
        INTERFACE_INCLUDE_DIRECTORIES "${CORE_ARTIFACT_INCLUDE_DIR}/core;${CORE_ARTIFACT_INCLUDE_DIR}/common"
    )

    message(STATUS "Imported common library: ${COMMON_LIB_PATH}")
    message(STATUS "Imported ic10_runtime_lib library: ${RUNTIME_LIB_PATH}")

    set(COMMON_INCLUDE_DIR "${CORE_ARTIFACT_INCLUDE_DIR}/common" CACHE PATH "Common include directory" FORCE)

else()
    message(STATUS "Building IC10 runtime core from source")
endif()
