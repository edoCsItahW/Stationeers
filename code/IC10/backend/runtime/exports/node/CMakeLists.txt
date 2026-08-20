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
        PRIVATE_LINK Common::Core Common::Node IC10::Compiler::Core IC10::Compiler::Node IC10::Runtime::Core Node::Node Node::NAPI
        LIBRARY_TYPE SHARED
        COMPILE_DEFINITIONS ${_node_compile_defs}
)

set_target_properties(ic10_runtime_node PROPERTIES PREFIX "" SUFFIX ".node" OUTPUT_NAME "ic10r-node")

st_apply_compile_options(ic10_runtime_node)

st_alias(IC10::Runtime::Node ic10_runtime_node)
