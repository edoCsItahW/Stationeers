# 创建运行时库目标
add_library(ic10_runtime_lib STATIC)

file(GLOB_RECURSE SOURCE CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.cpp"
)

target_sources(ic10_runtime_lib PRIVATE ${SOURCE})
target_include_directories(ic10_runtime_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${COMMON_INCLUDE_DIR})
target_link_libraries(ic10_runtime_lib PRIVATE ic10_lib common)

# 编译选项优化
if (MSVC)
    target_compile_options(ic10_runtime_lib PRIVATE /MP /W4)
    target_compile_options(ic10_runtime_lib PRIVATE $<$<NOT:$<CONFIG:Debug>>:/O1>)

elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(ic10_runtime_lib PRIVATE -O1 -Wall -Wextra)

elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(ic10_runtime_lib PRIVATE -O1 -Wall -Wextra)

endif ()
