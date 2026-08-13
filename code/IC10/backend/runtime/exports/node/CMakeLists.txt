#project(IC10_Runtime_Node VERSION 1.0)
#
## ============================================================================
## 依赖查找（Node.js）
## ============================================================================
#include(${CMAKE_CURRENT_SOURCE_DIR}/../../../../../common/cmake/node.cmake)  # 自动查找 Node.js 头文件和库
#
#
## ============================================================================
## 主要构建目标
## ============================================================================
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
## 需要编译器 export node 的头文件以访问 ProgramAdapter / SymbolTableAdapter
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../compiler/exports/node/include)
#file(GLOB COMPILER_NODE_SRC CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../../../compiler/exports/node/src/**/*.cpp)
#
#file(GLOB SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
#
#if(MSVC)
#    list(APPEND SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/win_delay_load_hook.cc")
#endif()
#
#add_library(${PROJECT_NAME} SHARED ${SOURCE} ${COMPILER_NODE_SRC})
#set_target_properties(${PROJECT_NAME} PROPERTIES
#        PREFIX ""
#        SUFFIX ".node"
#        OUTPUT_NAME "ic10r-node"
#)
#
#if(MSVC)
#    target_compile_definitions(${PROJECT_NAME} PRIVATE
#            HOST_BINARY="node.exe"
#    )
#    target_link_options(${PROJECT_NAME} PRIVATE
#            "/DELAYLOAD:node.exe"
#    )
#
#    target_link_libraries(${PROJECT_NAME}
#            PRIVATE
#            delayimp
#            Node::Node
#            ic10_runtime_lib
#            ic10_lib
#            common
#            common_node
#    )
#else()
#    target_link_libraries(${PROJECT_NAME}
#            PRIVATE
#            Node::Node
#            ic10_runtime_lib
#            ic10_lib
#            common
#            common_node
#    )
#endif()
include(CommonSettings)
include(Functions)


project(IC10_Runtime_Node VERSION 1.0)


st_source(IC10_RUNTIME_NODE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)
if (WIN32)
    list(APPEND IC10_RUNTIME_NODE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/win_delay_load_hook.cc)
endif ()

# IC10::Compiler::Node提供.node文件，其中符号没有`__declspec(dllexport)`，因此ic10_runtime_node链接它
# 并不能解析这些符号，因此需要特别链接源文件，同时去除build.cpp避免重复导出
st_source(IC10_COMPILER_NODE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../../../compiler/exports/node/src)
list(REMOVE_ITEM IC10_COMPILER_NODE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../../../compiler/exports/node/src/build.cpp)
list(APPEND IC10_RUNTIME_NODE_SRC ${IC10_COMPILER_NODE_SRC})

if (WIN32)
    set(_node_compile_defs HOST_BINARY="node.exe")
endif ()

st_library(ic10_runtime_node "${IC10_RUNTIME_NODE_SRC}" ${CMAKE_CURRENT_SOURCE_DIR}/include
        Common::Core Common::Node IC10::Compiler::Core IC10::Compiler::Node IC10::Runtime::Core Node::Node Node::NAPI
        LIBRARY_TYPE SHARED
        COMPILE_DEFINITIONS ${_node_compile_defs}
)

set_target_properties(ic10_runtime_node PROPERTIES PREFIX "" SUFFIX ".node" OUTPUT_NAME "ic10r-node")

st_apply_compile_options(ic10_runtime_node)

st_alias(IC10::Runtime::Node ic10_runtime_node)
