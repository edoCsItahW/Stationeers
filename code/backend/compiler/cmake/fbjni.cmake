# ============================================================================
# fbjni.cmake - 查找 Facebook JNI (fbjni) 库
# ============================================================================
#
# fbjni 默认构建为 SHARED 库，这会导致 ic10_java.dll 在运行时依赖
# fbjni.dll。为了让 fbjni 静态链接进 ic10_java.dll（简化分发），
# 这里在获取源码后将 CMakeLists.txt 中的 SHARED 改为 STATIC。

include(FetchContent)

# 检查是否已经存在，避免重复（fbjni.cmake 可能被 common_java 和 IC10_java 各 include 一次）
if(NOT TARGET fbjni)
    message(STATUS "fbjni.cmake: Fetching fbjni from GitHub... | fbjni.cmake：正在从 GitHub 获取 fbjni...")

    FetchContent_Declare(
            fbjni
            GIT_REPOSITORY https://github.com/facebookincubator/fbjni.git
            GIT_TAG        v0.7.0
    )

    if(NOT JAVA_HOME AND DEFINED ENV{JAVA_HOME})
        set(JAVA_HOME $ENV{JAVA_HOME})
    endif()

    set(FBJNI_SKIP_TESTS ON)

    # 手动 Populate（幂等：若已下载则仅设置变量），以便在构建前打补丁：SHARED -> STATIC
    FetchContent_Populate(fbjni)

    # 将 fbjni 从 SHARED 改为 STATIC，使其静态链接到 ic10_java.dll
    # 注意：string(REPLACE) 在已经是 STATIC 的情况下是幂等的（无匹配则无替换）
    file(READ "${fbjni_SOURCE_DIR}/CMakeLists.txt" _fbjni_cmake)
    string(REPLACE
        "add_library(fbjni SHARED"
        "add_library(fbjni STATIC"
        _fbjni_cmake "${_fbjni_cmake}")
    # 静态库没有链接器生成的 PDB 文件,移除 MSVC 下的 PDB 安装规则,
    # 否则 $<TARGET_PDB_FILE:fbjni> 生成器表达式在配置阶段报错
    string(REPLACE
        "install(FILES $<TARGET_PDB_FILE:fbjni> DESTINATION \${CMAKE_INSTALL_LIBDIR} OPTIONAL)"
        ""
        _fbjni_cmake "${_fbjni_cmake}")
    file(WRITE "${fbjni_SOURCE_DIR}/CMakeLists.txt" "${_fbjni_cmake}")

    add_subdirectory("${fbjni_SOURCE_DIR}" "${fbjni_BINARY_DIR}" EXCLUDE_FROM_ALL)

    # fbjni 默认会创建 fbjni 目标，如果未创建则报错
    if(NOT TARGET fbjni)
        message(FATAL_ERROR "fbjni target not created! | fbjni 目标未创建！")
    endif()

    # 静态链接时，FbjniApi.h 在未定义 BUILDING_FBJNI 时会使用 __declspec(dllimport)，
    # 这对静态库是错误的。将 BUILDING_FBJNI 暴露为 PUBLIC 定义，使所有使用 fbjni 的
    # 代码（如 ic10_java、common_java）都看到 __declspec(dllexport) 而非 dllimport。
    # 对于静态库，dllexport 实际被忽略，但能避免 dllimport 导致的链接错误。
    target_compile_definitions(fbjni PUBLIC BUILDING_FBJNI)

    message(STATUS "fbjni.cmake: fbjni fetched successfully (static). | fbjni.cmake：fbjni 获取成功（静态库）。")
else()
    message(STATUS "fbjni.cmake: fbjni already loaded. | fbjni.cmake：fbjni 已加载。")
endif()