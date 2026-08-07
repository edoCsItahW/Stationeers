# IC10 语言绑定导出
# 先添加公共绑定库（common_java, common_node, common_python）
add_subdirectory(../../../../common/cpp/export ${CMAKE_BINARY_DIR}/common_export)

# IC10 专属绑定
if(BUILD_JAVA_BINDINGS)
    add_subdirectory(java)
endif()

if(BUILD_NODE_BINDINGS)
    add_subdirectory(node)
endif()

if(BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()
