project(Common_Java_Build VERSION 1.0)

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

    add_compile_options("/wd4711")

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

# ============================================================================
# 构建共享库（JNI 动态库）
# ============================================================================
add_library(common_java STATIC)

target_sources(common_java PRIVATE ${SOURCE})

target_include_directories(common_java PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(common_java PUBLIC fbjni common)

add_library(${PROJECT_NAME} SHARED ${SOURCE})

set_target_properties(${PROJECT_NAME} PROPERTIES
        PREFIX ""                                 # 不加 lib 前缀
        # 输出到 build/exports/IC10/java/Release/ 下（Visual Studio 自动处理子目录）
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        common
        fbjni
        ${JNI_LIBRARIES}
)
