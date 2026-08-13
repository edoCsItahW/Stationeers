list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

include(Functions)
include(CommonSettings)

cmake_minimum_required(VERSION 3.28.1)
project(Stationeers VERSION 1.0)

set(CMAKE_CXX_STANDARD            23 )
set(CMAKE_CXX_EXTENSIONS          OFF)
set(CMAKE_CXX_STANDARD_REQUIRED   ON )
set(CMAKE_CXX_SCAN_FOR_MODULES    OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON )


st_l_options(ENABLE_CCACHE DC0 ON)
st_l_options(ENABLE_PAPALLEL DC1 ON)
st_l_options(ENABLE_ARTIFACTS DC2 ON)
st_l_options(ENABLE_DEP_CACHE DC3 ON)
# STATIONEERS_ARTIFACTS_DIR
# CMAKE_BUILD_PARALLEL_LEVEL

set(BUILD_CHILDREN IC10)  # AERO)
set(IC10_CHILDREN COMPILER RUNTIME)
set(COMPILER_CHILDREN CORE EXPORTS TESTS)
set(RUNTIME_CHILDREN CORE EXPORTS TESTS)
set(EXPORTS_CHILDREN NODE PYTHON JAVA)


resolve_feature_tree(BUILD)


if (ENABLE_ARTIFACTS AND (NOT DEFINED STATIONEERS_ARTIFACTS_DIR OR STATIONEERS_ARTIFACTS_DIR STREQUAL ""))
    set(STATIONEERS_ARTIFACTS_DIR "${CMAKE_SOURCE_DIR}/artifacts")
endif ()
if (ENABLE_DEP_CACHE AND (NOT DEFINED STATIONEERS_DEP_CACHE_DIR OR STATIONEERS_DEP_CACHE_DIR STREQUAL ""))
    set(STATIONEERS_DEP_CACHE_DIR "${CMAKE_SOURCE_DIR}/third-party")
endif ()

macro(st_auto_library TARGET SOURCES INCLUDE_DIR)
    enable_binary_cache("${ENABLE_ARTIFACTS}" "${STATIONEERS_ARTIFACTS_DIR}" "${TARGET}" "${SOURCES}" "${INCLUDE_DIR}" ${ARGN})

endmacro()


if (ANY_BUILD)
    if (ENABLE_CCACHE)
        enable_compiler_cache()
    endif ()

    if (ENABLE_PAPALLEL AND NOT DEFINED CMAKE_BUILD_PARALLEL_LEVEL)
        set(CMAKE_BUILD_PARALLEL_LEVEL 4)
    endif ()

    if (ANY_NODE)
        find_package(NodeAddonApi REQUIRED)
    endif ()

    if (ANY_JAVA)
        set(FBJNI_INSTALL_PREFIX "${STATIONEERS_DEP_CACHE_DIR}")

        set(FBJNI_SKIP_TESTS ON)

        if (WIN32)
            find_package(fbjni REQUIRED)
        else ()
            find_package(Fbjni REQUIRED)
        endif ()
    endif ()

    if (ANY_PYTHON)
        if (WIN32)
            find_package(pybind11 REQUIRED)
        else ()
            find_package(Pybind11 REQUIRED)
        endif ()
    endif ()

    if (ANY_TESTS)
        set(GOOGLETEST_INSTALL_PREFIX "${STATIONEERS_DEP_CACHE_DIR}")

        find_package(GoogleTest REQUIRED)
    endif ()

    add_subdirectory(common)

endif ()

if (ANY_IC10)
    add_subdirectory(IC10)
endif ()


