project(IC10_Java_API VERSION 1.0)

# 确保 JNI 头文件路径包含 win32 子目录
if (WIN32)
    # 从环境变量或缓存中获取 JAVA_HOME
    if (NOT DEFINED JAVA_HOME)
        set(JAVA_HOME $ENV{JAVA_HOME})
        string(REPLACE "\\" "/" JAVA_HOME "${JAVA_HOME}")
    endif()
    if (NOT JAVA_HOME)
        message(FATAL_ERROR "JAVA_HOME is not set. Please set it to your JDK installation path.")
    endif()
    # 将 JDK 的 include 路径添加到全局包含目录（所有目标继承）
    include_directories("${JAVA_HOME}/include" "${JAVA_HOME}/include/win32")
endif()

# ============================================================================
# 依赖查找（JNI 和 fbjni）
# ============================================================================
find_package(JNI REQUIRED)                    # 标准 JNI 头文件（jni.h）
include(${CMAKE_SOURCE_DIR}/cmake/fbjni.cmake) # 获取 fbjni C++ 库

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# ============================================================================
# 源文件收集
# ============================================================================
file(GLOB_RECURSE SOURCE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Windows 下特有的延迟加载钩子（如果你需要，参考 node 写法）
if(MSVC)
    # 如果你的 jni cpp 里用到了延迟加载，可以加下面这行，否则注释掉
    # list(APPEND SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/win_delay_load_hook.cc")
endif()

# ============================================================================
# 构建共享库（JNI 动态库）
# ============================================================================
add_library(${PROJECT_NAME} SHARED ${SOURCE})

# 只指定输出名，输出目录交给 CMake 默认 build 路径
# 工件移动由 scripts/IC10JavaExport.ps1 负责（复制到 tests/java/ic10/libs）
set_target_properties(${PROJECT_NAME} PROPERTIES
        PREFIX ""
        OUTPUT_NAME "ic10_java"
)

# ============================================================================
# 链接依赖
# ============================================================================
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        fbjni                     # fbjni C++ 库
        ${JNI_LIBRARIES}          # JVM 相关库（Windows 下为 jvm.lib）
        ic10_lib                  # IC10 核心静态库（来自根目录或 artifact）
        common                    # 公共基础库
        common_java
)
