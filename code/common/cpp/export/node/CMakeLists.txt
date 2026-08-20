project(Common_Node VERSION 1.0)


st_source(COMMON_NODE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")
st_auto_library(common_node "${COMMON_NODE_SRC}" "${CMAKE_CURRENT_SOURCE_DIR}/include" PRIVATE_LINK Common::Core Node::Node Node::NAPI)

set_target_properties(common_node PROPERTIES PREFIX "" SUFFIX ".node")

st_apply_compile_options(common_node)

st_alias(Common::Node common_node)
