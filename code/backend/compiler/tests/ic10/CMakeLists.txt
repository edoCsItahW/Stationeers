# 创建针对 IC10 编译器的测试可执行文件
add_executable(ic10_tests
        test_lexer.cpp
        test_parser.cpp
)

# 链接必要的库
target_link_libraries(ic10_tests
        PRIVATE
        common                # 公共基础库
        ic10_lib              # 链接静态库
        gtest_main            # GoogleTest 主函数
)


# 指定包含路径（如有需要）
target_include_directories(ic10_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/IC10/include
        ${CMAKE_SOURCE_DIR}/common/include
)

# 添加测试
add_test(NAME ic10_tests COMMAND ic10_tests)

# 为 IC10 测试单独设置超时、工作目录等
set_tests_properties(ic10_tests PROPERTIES
        TIMEOUT 30
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)