# IC10 运行时测试可执行文件
add_executable(ic10_runtime_tests
        cpp/test_memory.cpp
        cpp/test_executor.cpp
        cpp/test_engine.cpp
        cpp/test_integration.cpp
        cpp/test_system.cpp
)

# 链接必要的库
target_link_libraries(ic10_runtime_tests
        PRIVATE
        ic10_runtime_lib      # 运行时核心库
        ic10_lib              # 编译器核心库（AST 类型等）
        common                # 公共基础库
        gtest_main            # GoogleTest 主函数
)

# 包含路径
target_include_directories(ic10_runtime_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/core/include
        ${CMAKE_SOURCE_DIR}/../compiler/core/include
        ${CMAKE_SOURCE_DIR}/../../../common/cpp/core/include
)

# 添加测试
add_test(NAME ic10_runtime_tests COMMAND ic10_runtime_tests)

# 超时、工作目录等
set_tests_properties(ic10_runtime_tests PROPERTIES
        TIMEOUT 120
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
