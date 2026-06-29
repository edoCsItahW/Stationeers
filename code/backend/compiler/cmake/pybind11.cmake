# ============================================================================
# pybind11.cmake - 查找 Python 和 pybind11（用于 C++/Python 绑定）
# ============================================================================

# 1. 查找 Python 解释器及开发库（必须）
find_package(Python REQUIRED COMPONENTS Interpreter Development)

# 2. 尝试从 Python 环境中获取 pybind11 的 CMake 配置目录
if(NOT pybind11_DIR)
    message(STATUS "pybind11.cmake: Attempting to detect pybind11 via Python... | pybind11.cmake：正在通过 Python 检测 pybind11...")
    execute_process(
            COMMAND ${Python_EXECUTABLE} -c "import pybind11; print(pybind11.get_cmake_dir())"
            OUTPUT_VARIABLE _PYBIND11_CMAKE_DIR
            RESULT_VARIABLE _PYBIND11_RESULT
            ERROR_VARIABLE  _PYBIND11_ERROR
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(_PYBIND11_RESULT EQUAL 0 AND _PYBIND11_CMAKE_DIR)
        file(TO_CMAKE_PATH "${_PYBIND11_CMAKE_DIR}" _PYBIND11_CMAKE_DIR)
        set(pybind11_DIR "${_PYBIND11_CMAKE_DIR}" CACHE PATH "Path to pybind11 CMake config")
        message(STATUS "pybind11.cmake: auto-detected pybind11_DIR = ${pybind11_DIR} | pybind11.cmake：自动检测到 pybind11_DIR = ${pybind11_DIR}")
    else()
        message(STATUS "pybind11.cmake: Detection failed (result=${_PYBIND11_RESULT}, error='${_PYBIND11_ERROR}')")
        # 若未检测到，则让后续 find_package 尝试默认路径或报错
    endif()
endif()

# 3. 查找 pybind11 配置文件（若 pybind11_DIR 已设置，则优先使用）
find_package(pybind11 REQUIRED CONFIG)

# 4. 验证导入目标是否存在
if(NOT TARGET pybind11::pybind11)
    message(FATAL_ERROR "pybind11::pybind11 target not created. Please install pybind11 (`pip install pybind11`) or set pybind11_DIR. | pybind11::pybind11 目标未创建。请安装 pybind11（`pip install pybind11`）或设置 pybind11_DIR。")
endif()

# 5. 导出 pybind11 包含目录（供需要手动 include 的场景使用，但推荐通过 target 自动传递）
get_target_property(PYBIND11_INCLUDE_DIRS pybind11::pybind11 INTERFACE_INCLUDE_DIRECTORIES)
if(PYBIND11_INCLUDE_DIRS)
    message(STATUS "pybind11.cmake <PYBIND11_INCLUDE_DIRS>: ${PYBIND11_INCLUDE_DIRS} | <PYBIND11_INCLUDE_DIRS>：${PYBIND11_INCLUDE_DIRS}")
else()
    message(STATUS "pybind11.cmake <PYBIND11_INCLUDE_DIRS>: (not available)")
endif()

# 6. 输出诊断信息（与 node.cmake 风格统一）
message(STATUS "pybind11.cmake <Python_EXECUTABLE>: ${Python_EXECUTABLE} | <Python_EXECUTABLE>：${Python_EXECUTABLE}")
message(STATUS "pybind11.cmake <pybind11_DIR>: ${pybind11_DIR} | <pybind11_DIR>：${pybind11_DIR}")
message(STATUS "pybind11.cmake <NOTE>: The target 'pybind11::pybind11' automatically provides Python headers and libraries | <NOTE>：目标 'pybind11::pybind11' 自动提供 Python 头文件和库")