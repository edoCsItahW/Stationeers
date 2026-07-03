project(IC10_Python_API VERSION 1.0)

# ============================================================================
# 依赖查找（pybind11）
# ============================================================================
include(${CMAKE_SOURCE_DIR}/cmake/pybind11.cmake)

# ============================================================================
# 主要构建目标
# ============================================================================
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

pybind11_add_module(ic10_python ${SOURCE})

# 设置输出文件后缀为.pyd,去掉pybind11默认添加的ABI标签(如.cp313-win_amd64)
# Set output suffix to .pyd, removing pybind11's default ABI tag (e.g. .cp313-win_amd64)
set_target_properties(ic10_python PROPERTIES SUFFIX ".pyd")

# 链接依赖库
# Link dependencies
target_link_libraries(ic10_python
        PRIVATE
        common_python        # common Python适配器(Pos/Error)
        ic10_lib             # IC10编译器核心库
        common               # 公共库
)
