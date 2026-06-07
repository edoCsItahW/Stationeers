# 公共基础库，静态库形式
add_library(common STATIC)

target_include_directories(common
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)


# 使用 lld
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_link_options(common INTERFACE -fuse-ld=lld)
endif()

# 收集源文件
file(GLOB COMMON_SOURCES src/**/*.cpp)
target_sources(common PRIVATE ${COMMON_SOURCES})

# 预编译头
if(MSVC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    file(GLOB_RECURSE PCH_HEADERS
            "${CMAKE_CURRENT_SOURCE_DIR}/include/ic10/pch/*.hpp"
    )
    target_precompile_headers(common PRIVATE ${PCH_HEADERS})
endif()

# 设置 C++ 标准（继承顶层）
set_target_properties(common PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
)

# 导出 common 的依赖（如需要 std::coroutine）
target_compile_features(common PUBLIC cxx_std_23)