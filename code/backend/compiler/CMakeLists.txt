cmake_minimum_required(VERSION 3.28.1)

project(Compiler VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(LLVM_CCACHE_BUILD ON CACHE BOOL "Use ccache")
endif()

# 全局包含路径（供子项目使用公共头文件）
set(COMMON_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/common/include)

# 全局编译选项
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")

    add_compile_options(-ftime-trace)

endif ()


# 子目录
add_subdirectory(common)
add_subdirectory(IC10)
add_subdirectory(StationScript)
