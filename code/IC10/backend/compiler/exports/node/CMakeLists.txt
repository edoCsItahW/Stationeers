include(Functions)


project(IC10_Compiler_Node VERSION 1.0)


st_source(IC10_COMPILER_NODE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)
if (WIN32)
    list(APPEND IC10_COMPILER_NODE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/win_delay_load_hook.cc)
endif ()

if (WIN32)
    set(_node_compile_defs HOST_BINARY="node.exe")
endif ()

st_auto_library(ic10_compiler_node "${IC10_COMPILER_NODE_SRC}" ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE_LINK Common::Core IC10::Compiler::Core Node::Node Node::NAPI Common::Node
        LIBRARY_TYPE SHARED
        COMPILE_DEFINITIONS ${_node_compile_defs}
)

set_target_properties(ic10_compiler_node PROPERTIES PREFIX "" SUFFIX ".node" OUTPUT_NAME "ic10c-node")

st_apply_compile_options(ic10_compiler_node)

st_alias(IC10::Compiler::Node ic10_compiler_node)