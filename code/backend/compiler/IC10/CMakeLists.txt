add_executable(ic10_compiler)

# 头文件路径
target_include_directories(ic10_compiler
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${COMMON_INCLUDE_DIR}          # 公共库头文件
)

# 源文件：所有 .cpp
file(GLOB_RECURSE COMPILER_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
target_sources(ic10_compiler PRIVATE ${COMPILER_SOURCES})

# 链接公共库
target_link_libraries(ic10_compiler PRIVATE common)

# 预编译头
if(MSVC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    file(GLOB_RECURSE PCH_HEADERS
            "${CMAKE_CURRENT_SOURCE_DIR}/include/ic10/pch/*.hpp"
    )
    target_precompile_headers(ic10_compiler PRIVATE ${PCH_HEADERS})
endif()

# 编译选项
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(ic10_compiler PRIVATE -Wa,-mbig-obj)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(ic10_compiler PRIVATE -Wa,-mbig-obj)
endif()

# 设置可执行文件输出目录
set_target_properties(ic10_compiler PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)