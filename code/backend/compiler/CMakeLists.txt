cmake_minimum_required(VERSION 3.28.1)

project(Compiler VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# 全局编译选项
if (MSVC)

    add_compile_options(/wd4819 /utf-8 /diagnostics:caret /bigobj)

elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")

    add_compile_options(-ftime-trace)

endif ()

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(LLVM_CCACHE_BUILD ON CACHE BOOL "Use ccache")
endif()

# 全局包含路径（供子项目使用公共头文件）
set(COMMON_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/common/include)

# 开启测试
enable_testing()

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.17.0
)
FetchContent_MakeAvailable(googletest)

# 子目录
add_subdirectory(common)
add_subdirectory(IC10)
add_subdirectory(StationScript)
add_subdirectory(exports)
add_subdirectory(tests)
