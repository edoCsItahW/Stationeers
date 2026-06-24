project(IC10_Node_API VERSION 1.0)

# ============================================================================
# 依赖查找（pybind11 和 Node.js）
# ============================================================================
include(${CMAKE_SOURCE_DIR}/cmake/node.cmake)       # 自动查找 Node.js 头文件和库

# ============================================================================
# 主要构建目标
# ============================================================================
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_library(${PROJECT_NAME} SHARED ${SOURCE})
set_target_properties(${PROJECT_NAME} PROPERTIES
        PREFIX ""                     # 无前缀（直接生成 .node 文件）
        SUFFIX ".node"                # Node.js 原生模块后缀
        OUTPUT_NAME "ic10-node-api"   # 指定最终文件名（不含前缀后缀）
)

# 链接依赖库
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        Node::Node                # 来自 node.cmake（Windows 下为 node.lib）
        ic10_lib
        common
        common_node
)
