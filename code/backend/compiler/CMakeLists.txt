cmake_minimum_required(VERSION 3.28.1)

project(Compiler VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

option(BUILD_IC10 "Build IC10 compiler core" ON)
option(BUILD_STATIONSCRIPT "Build StationScript compiler" ON)
option(BUILD_NODE_BINDINGS "Build Node.js bindings" ON)
option(BUILD_PYTHON_BINDINGS "Build Python bindings" OFF)
option(BUILD_TESTS "Build tests" ON)
option(ENABLE_CCACHE "Enable ccache/sccache if available" ON)

# 全局编译选项
if (MSVC)

    add_compile_options(/wd4819 /utf-8 /diagnostics:caret /bigobj)

elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")

    add_compile_options(-ftime-trace)

endif ()

if(ENABLE_CCACHE)
    find_program(CCACHE_PROGRAM ccache)
    find_program(SCCACHE_PROGRAM sccache)
    if(SCCACHE_PROGRAM)
        set(CMAKE_C_COMPILER_LAUNCHER ${SCCACHE_PROGRAM})
        set(CMAKE_CXX_COMPILER_LAUNCHER ${SCCACHE_PROGRAM})
        message(STATUS "Using sccache for compilation caching")
    elseif(CCACHE_PROGRAM)
        set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
        set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
        set(LLVM_CCACHE_BUILD ON CACHE BOOL "Use ccache")
        message(STATUS "Using ccache for compilation caching")
    endif()
endif()

# 全局包含路径（供子项目使用公共头文件）
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

if(BUILD_STATIONSCRIPT)
    add_subdirectory(StationScript)
endif()

if(BUILD_NODE_BINDINGS OR BUILD_PYTHON_BINDINGS)
    add_subdirectory(exports)
endif()

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
