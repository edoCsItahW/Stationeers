project(IC10_Node_API VERSION 1.0)

# ============================================================================
# 依赖查找（pybind11 和 Node.js）
# ============================================================================
option(USE_ELECTRON "Build with Electron headers instead of Node.js" OFF)
if(USE_ELECTRON)
    include(${CMAKE_SOURCE_DIR}/cmake/electron_node.cmake)
else()
    include(${CMAKE_SOURCE_DIR}/cmake/node.cmake)  # 自动查找 Node.js 头文件和库
endif()

# ============================================================================
# 主要构建目标
# ============================================================================
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

if(MSVC)
    list(APPEND SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/win_delay_load_hook.cc")
endif()

add_library(${PROJECT_NAME} SHARED ${SOURCE})
set_target_properties(${PROJECT_NAME} PROPERTIES
        PREFIX ""
        SUFFIX ".node"
        OUTPUT_NAME "ic10-node-api"
)

if(MSVC)
    if(USE_ELECTRON)
        target_compile_definitions(${PROJECT_NAME} PRIVATE
                HOST_BINARY="electron.exe"
        )
        target_link_options(${PROJECT_NAME} PRIVATE
                "/DELAYLOAD:electron.exe"
        )
    else()
        target_compile_definitions(${PROJECT_NAME} PRIVATE
                HOST_BINARY="node.exe"
        )
        target_link_options(${PROJECT_NAME} PRIVATE
                "/DELAYLOAD:node.exe"
        )
    endif()
    target_link_libraries(${PROJECT_NAME}
            PRIVATE
            delayimp
            Node::Node
            ic10_lib
            common
            common_node
    )
else()
    target_link_libraries(${PROJECT_NAME}
            PRIVATE
            Node::Node
            ic10_lib
            common
            common_node
    )
endif()
