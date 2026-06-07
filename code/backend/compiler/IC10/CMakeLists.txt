add_executable(IC10_Compiler)

# 头文件路径
target_include_directories(IC10_Compiler
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${COMMON_INCLUDE_DIR}          # 公共库头文件
)

target_compile_definitions(IC10_Compiler PRIVATE IC10_SIMPLE_MODE)

# 源文件：所有 .cpp
file(GLOB_RECURSE COMPILER_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)
target_sources(IC10_Compiler PRIVATE ${COMPILER_SOURCES})

# 链接公共库
target_link_libraries(IC10_Compiler PRIVATE common)

# 预编译头
if (MSVC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    file(GLOB_RECURSE PCH_HEADERS
            "${CMAKE_CURRENT_SOURCE_DIR}/include/ic10/pch/*.hpp"
    )
    target_precompile_headers(IC10_Compiler PRIVATE ${PCH_HEADERS})
endif ()

# 编译选项
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(IC10_Compiler PRIVATE -Wa,-mbig-obj)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(IC10_Compiler PRIVATE -Wa,-mbig-obj)
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "GUN")

    target_link_options(IC10_Compiler PRIVATE -static-libstdc++ -static-libgcc)

elseif (MINGW)

    target_link_options(IC10_Compiler PRIVATE -static-libstdc++ -static-libgcc)

endif ()

# 设置可执行文件输出目录
set_target_properties(IC10_Compiler PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)