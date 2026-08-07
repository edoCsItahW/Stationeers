cmake_minimum_required(VERSION 3.28.1)

project(IC10_Runtime VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 性能优化：启用多线程编译
if(NOT DEFINED CMAKE_BUILD_PARALLEL_LEVEL)
    set(CMAKE_BUILD_PARALLEL_LEVEL 4)
endif()

option(BUILD_IC10_RUNTIME "Build IC10 runtime core" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_RUNTIME_NODE_BINDINGS "Build Node.js bindings" ON)
option(BUILD_RUNTIME_PYTHON_BINDINGS "Build Python bindings" OFF)
option(BUILD_RUNTIME_JAVA_BINDINGS  "Build Java bindings" OFF)
option(ENABLE_CCACHE "Enable ccache/sccache if available" ON)

if (BUILD_RUNTIME_NODE_BINDINGS)
    set(BUILD_NODE_BINDINGS ON CACHE BOOL "Build Common Node.js bindings" FORCE)
elseif (BUILD_RUNTIME_PYTHON_BINDINGS)
    set(BUILD_PYTHON_BINDINGS OFF CACHE BOOL "Build Common Python bindings" FORCE)
elseif (BUILD_RUNTIME_JAVA_BINDINGS)
    set(BUILD_JAVA_BINDINGS ON CACHE BOOL "Build Common Java bindings" FORCE)
endif ()

# 全局编译选项
if (MSVC)
    string(REPLACE "/O2" "/O1" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    add_compile_options(/wd4819 /utf-8 /diagnostics:caret /bigobj /MP /Zc:__cplusplus)
    add_compile_options($<$<CONFIG:Debug>:/Od>)

elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-ftime-trace)
    add_compile_options($<$<CONFIG:Release>:-O1>)
    add_compile_options($<$<CONFIG:Debug>:-O0>)

endif ()

if(ENABLE_CCACHE)
    find_program(CCACHE_PROGRAM ccache)
    find_program(SCCACHE_PROGRAM sccache)
    if(SCCACHE_PROGRAM)
        set(CMAKE_C_COMPILER_LAUNCHER ${SCCACHE_PROGRAM})
        set(CMAKE_CXX_COMPILER_LAUNCHER ${SCCACHE_PROGRAM})
        message(STATUS "Using sccache for compilation caching | 使用 sccache 加速编译")
    elseif(CCACHE_PROGRAM)
        set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
        set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
        set(LLVM_CCACHE_BUILD ON CACHE BOOL "Use ccache")
        message(STATUS "Using ccache for compilation caching")
    endif()
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/core_artifact.cmake)

if(NOT IC10_RUNTIME_CORE_ARTIFACT_DIR OR NOT EXISTS "${IC10_RUNTIME_CORE_ARTIFACT_DIR}")
    set(COMMON_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../common/cpp/core/include)

    # 开启测试
    if(BUILD_TESTS)
        enable_testing()

        include(FetchContent)
        FetchContent_Declare(
                googletest
                GIT_REPOSITORY https://github.com/google/googletest.git
                GIT_TAG        v1.17.0
        )
        FetchContent_MakeAvailable(googletest)
    endif()

    # 子目录
    if(NOT TARGET common)
        add_subdirectory(../../../common/cpp ${CMAKE_BINARY_DIR}/common)
    endif()
    if(NOT TARGET ic10_lib)
        add_subdirectory(../compiler/core ${CMAKE_BINARY_DIR}/compiler_core)
    endif()

    if(BUILD_IC10_RUNTIME)
        add_subdirectory(core)
    endif()
else()
    if(BUILD_TESTS)
        message(WARNING "BUILD_TESTS is ON but using prebuilt core artifact - tests may not work correctly")
    endif()
endif()

if(BUILD_RUNTIME_NODE_BINDINGS OR BUILD_RUNTIME_PYTHON_BINDINGS OR BUILD_RUNTIME_JAVA_BINDINGS)
    add_subdirectory(exports)
endif()

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
