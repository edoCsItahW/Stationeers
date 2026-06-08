# 开启测试
enable_testing()

# 添加共享工具子目录
add_subdirectory(common)

# 添加各子项目的测试目录
add_subdirectory(ic10)
add_subdirectory(station_script)