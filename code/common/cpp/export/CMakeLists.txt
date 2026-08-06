# 公共语言绑定库（common_java, common_node, common_python）
if(BUILD_JAVA_BINDINGS)
    add_subdirectory(java)
endif()

if(BUILD_NODE_BINDINGS)
    add_subdirectory(node)
endif()

if(BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()
