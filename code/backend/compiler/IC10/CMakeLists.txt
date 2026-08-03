# 创建库目标
add_library(ic10_lib STATIC)

# 源文件：所有 .cpp
file(GLOB_RECURSE COMPILER_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.cpp"
)

target_sources(ic10_lib PRIVATE ${COMPILER_SOURCES})
target_include_directories(ic10_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${COMMON_INCLUDE_DIR})
target_link_libraries(ic10_lib PRIVATE common)

# 预编译头（同时为静态库和可执行文件启用）
if (MSVC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    file(GLOB_RECURSE PCH_HEADERS
            "${CMAKE_CURRENT_SOURCE_DIR}/include/ic10/pch/*.hpp"
    )
    target_precompile_headers(ic10_lib PRIVATE ${PCH_HEADERS})
endif ()

# 编译选项优化
if (MSVC)
    target_compile_options(ic10_lib PRIVATE /MP /O1 /W4)

elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(ic10_lib PRIVATE -O1 -Wall -Wextra)

elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(ic10_lib PRIVATE -O1 -Wall -Wextra)

endif ()

# 保留可执行文件（单独编译 main.cpp）
add_executable(IC10_Compiler main.cpp)
target_link_libraries(IC10_Compiler PRIVATE ic10_lib common)

# 头文件路径
target_include_directories(IC10_Compiler
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${COMMON_INCLUDE_DIR}
)

target_compile_definitions(IC10_Compiler PRIVATE IC10_SIMPLE_MODE)

# 可执行文件编译选项
if (MSVC)
    target_compile_options(IC10_Compiler PRIVATE /MP /O1 /W4)
    target_link_options(IC10_Compiler PRIVATE LINKER:/pdbpagesize:16384)
    add_compile_options(/wd4514)

elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(IC10_Compiler PRIVATE -O1 -Wall -Wextra)
    target_link_options(IC10_Compiler PRIVATE LINKER:/pdbpagesize:16384)

elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(IC10_Compiler PRIVATE -O1 -Wall -Wextra)
    target_link_options(IC10_Compiler PRIVATE -static-libstdc++ -static-libgcc)

elseif (MINGW)
    target_link_options(IC10_Compiler PRIVATE -static-libstdc++ -static-libgcc)

endif ()

# PCH重用
if (MSVC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_precompile_headers(IC10_Compiler REUSE_FROM ic10_lib)
endif ()

# 设置可执行文件输出目录
set_target_properties(IC10_Compiler PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)