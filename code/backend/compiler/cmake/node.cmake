# ============================================================================
# node.cmake - 查找 Node.js 头文件和库（用于 N-API 原生模块）
# ============================================================================

# 1. 查找 Node.js 可执行文件
find_program(NODE_EXECUTABLE node)
if(NOT NODE_EXECUTABLE)
    message(FATAL_ERROR "Node.js not found. Please install Node.js and ensure 'node' is in PATH.")
endif()

# 2. 获取 Node.js 版本号（去掉前缀 'v'）
execute_process(
        COMMAND ${NODE_EXECUTABLE} -p "process.version.slice(1)"
        OUTPUT_VARIABLE NODE_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

# 3. 构造 node-gyp 缓存根目录（Windows 使用 LOCALAPPDATA，Unix 使用 ~/.cache）
if(WIN32)
    set(NODE_GYP_CACHE_ROOT "$ENV{LOCALAPPDATA}/node-gyp/Cache")
else()
    set(NODE_GYP_CACHE_ROOT "$ENV{HOME}/.cache/node-gyp")
endif()

# 4. 最终 Node 开发文件目录
set(NODE_DIR "${NODE_GYP_CACHE_ROOT}/${NODE_VERSION}")

# 5. 头文件目录（node-gyp 缓存结构）
set(NODE_INCLUDE_DIRS "${NODE_DIR}/include/node")

# 6. 库文件路径（Windows 下为 Release/node.lib，Unix 下留空）
if(WIN32)
    set(NODE_LIBRARIES "${NODE_DIR}/x64/node.lib")
else()
    set(NODE_LIBRARIES "")   # Unix 通常不需要静态链接 libnode
endif()

# 7. 获取 node-addon-api 的头文件路径（通过 require 获取）
execute_process(
        COMMAND ${NODE_EXECUTABLE} -p "require('node-addon-api').include"
        OUTPUT_VARIABLE NAPI_INCLUDE_DIRS
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
)
if(NOT NAPI_INCLUDE_DIRS)
    message(FATAL_ERROR "node-addon-api not found. Please run 'npm install node-addon-api' in your project root.")
endif()

# 8. 输出诊断信息
message(STATUS "node.cmake <NODE_DIR>: ${NODE_DIR}")
message(STATUS "node.cmake <NODE_INCLUDE_DIRS>: ${NODE_INCLUDE_DIRS}")
message(STATUS "node.cmake <NODE_LIBRARIES>: ${NODE_LIBRARIES}")
message(STATUS "node.cmake <NAPI_INCLUDE_DIRS>: ${NAPI_INCLUDE_DIRS}")

string(REPLACE "\"" "" NAPI_INCLUDE_DIRS "${NAPI_INCLUDE_DIRS}")
file(TO_CMAKE_PATH "${NAPI_INCLUDE_DIRS}" NAPI_INCLUDE_DIRS)

# 9. 添加头文件搜索路径
include_directories(${NODE_INCLUDE_DIRS})
include_directories(${NAPI_INCLUDE_DIRS})

# 10. 定义 IMPORTED 库目标（供链接使用）
if(NOT TARGET Node::Node)
    add_library(Node::Node UNKNOWN IMPORTED)
    set_target_properties(Node::Node PROPERTIES
            IMPORTED_LOCATION          "${NODE_LIBRARIES}"
            INTERFACE_INCLUDE_DIRECTORIES "${NODE_INCLUDE_DIRS}"
    )
endif()