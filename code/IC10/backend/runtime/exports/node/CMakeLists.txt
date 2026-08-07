project(IC10_Runtime_Node VERSION 1.0)

# ============================================================================
# 依赖查找（Node.js）
# ============================================================================
include(${CMAKE_CURRENT_SOURCE_DIR}/../../../../../common/cmake/node.cmake)  # 自动查找 Node.js 头文件和库


# ============================================================================
# 主要构建目标
# ============================================================================
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
# 需要编译器 export node 的头文件以访问 ProgramAdapter / SymbolTableAdapter
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../compiler/exports/node/include)
file(GLOB COMPILER_NODE_SRC CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../../../compiler/exports/node/src/**/*.cpp)

file(GLOB SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

if(MSVC)
    list(APPEND SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/win_delay_load_hook.cc")
endif()

add_library(${PROJECT_NAME} SHARED ${SOURCE} ${COMPILER_NODE_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES
        PREFIX ""
        SUFFIX ".node"
        OUTPUT_NAME "ic10r-node"
)

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            HOST_BINARY="node.exe"
    )
    target_link_options(${PROJECT_NAME} PRIVATE
            "/DELAYLOAD:node.exe"
    )

    target_link_libraries(${PROJECT_NAME}
            PRIVATE
            delayimp
            Node::Node
            ic10_runtime_lib
            ic10_lib
            common
            common_node
    )
else()
    target_link_libraries(${PROJECT_NAME}
            PRIVATE
            Node::Node
            ic10_runtime_lib
            ic10_lib
            common
            common_node
    )
endif()
