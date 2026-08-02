cmake_minimum_required(VERSION 3.28.1)

project(Compiler VERSION 2.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 性能优化：启用多线程编译
if(NOT DEFINED CMAKE_BUILD_PARALLEL_LEVEL)
    set(CMAKE_BUILD_PARALLEL_LEVEL 4)
endif()

option(BUILD_IC10 "Build IC10 compiler core" ON)
option(BUILD_STATIONSCRIPT "Build StationScript compiler" ON)
option(BUILD_NODE_BINDINGS "Build Node.js bindings" ON)
option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(BUILD_JAVA_BINDINGS  "Build Java bindings" ON)
option(BUILD_TESTS "Build tests" ON)
option(ENABLE_CCACHE "Enable ccache/sccache if available" ON)

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

if(NOT IC10_CORE_ARTIFACT_DIR OR NOT EXISTS "${IC10_CORE_ARTIFACT_DIR}")
    set(COMMON_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/common/include)

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
    add_subdirectory(common)

    if(BUILD_IC10)
        add_subdirectory(IC10)
    endif()
else()
    if(BUILD_TESTS)
        message(WARNING "BUILD_TESTS is ON but using prebuilt core artifact - tests may not work correctly")
    endif()
endif()

if(BUILD_STATIONSCRIPT)
    add_subdirectory(StationScript)
endif()

if(BUILD_NODE_BINDINGS OR BUILD_PYTHON_BINDINGS OR BUILD_JAVA_BINDINGS)
    add_subdirectory(exports)
endif()

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()