include(${CMAKE_CURRENT_LIST_DIR}/modules/Dependencies.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/modules/Locale.cmake)


find_package(Java REQUIRED)
find_package(JNI REQUIRED)

if (NOT Java_JAVA_HOME AND DEFINED ENV{JAVA_HOME})
    st_localize(DFbjni0 _JDK_R_DOC)
    set(Java_JAVA_HOME "$ENV{JAVA_HOME}" CACHE PATH ${_JDK_R_DOC} FORCE)
endif ()

if (Java_JAVA_HOME)
    st_localize(DFbjni1 _JDK_IR_DOC)
    set(JAVA_HOME "${Java_JAVA_HOME}" CACHE PATH ${_JDK_IR_DOC} FORCE)

else ()
    st_l_warning(WFbjni2)

endif ()


st_localize(DFbjni3 _INSTALL_DOC)
option(FBJNI_AUTO_INSTALL ${_INSTALL_DOC} ON)

st_localize(DFbjni4 _GIT_TAG_DOC)
set(FBJNI_GIT_TAG "v0.7.0" CACHE STRING ${_GIT_TAG_DOC})

st_localize(DFbjni5 _INSTALL_PREFIX_DOC)
set(FBJNI_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/third-party" CACHE PATH ${_INSTALL_PREFIX_DOC})


# -------------------- 查找现有 fbjni 源码目录 --------------------
# 源码方式检测（非 CONFIG 模式）：fbjni 通过 add_subdirectory 构建，
# 不会生成 CMake config 包，因此不能用 find_package(fbjni CONFIG) 查找。
# 这里检测 fbjni-src 目录是否已存在，避免重复下载。
st_localize(DFbjni6 _FBJNI_DIR_DOC)

if (NOT fbjni_DIR AND NOT DEFINED ENV{FBJNI_DIR})
    set(_fbjni_search_paths
            "${CMAKE_SOURCE_DIR}/third-party"
            "${FBJNI_INSTALL_PREFIX}"
            "/usr/local"
            "/usr"
    )

    foreach (_path ${_fbjni_search_paths})
        if (EXISTS "${_path}/fbjni-src/CMakeLists.txt")
            set(fbjni_DIR "${_path}/fbjni-src" CACHE PATH ${_FBJNI_DIR_DOC})
            break()
        endif ()
    endforeach ()

endif ()

# 检查 FBJNI_DIR 环境变量
if (NOT fbjni_DIR AND DEFINED ENV{FBJNI_DIR})
    if (EXISTS "$ENV{FBJNI_DIR}/CMakeLists.txt")
        set(fbjni_DIR "$ENV{FBJNI_DIR}" CACHE PATH ${_FBJNI_DIR_DOC})
    endif ()
endif ()


# -------------------- 构建或复用 fbjni --------------------
set(_fbjni_source_dir "")
set(_fbjni_binary_dir "${CMAKE_BINARY_DIR}/dependencies/fbjni-build")

if (NOT TARGET fbjni)
    if (fbjni_DIR AND EXISTS "${fbjni_DIR}/CMakeLists.txt")
        # 源码已存在——跳过下载（检测以避免重复下载）
        st_l_info_fmt(IFbjni0 ${fbjni_DIR})
        set(_fbjni_source_dir "${fbjni_DIR}")

    elseif (FBJNI_AUTO_INSTALL)
        # 下载并构建
        st_l_info_fmt(IFbjni1 ${FBJNI_GIT_TAG})

        include(FetchContent)
        FetchContent_Declare(fbjni
                GIT_REPOSITORY https://github.com/facebookincubator/fbjni.git
                GIT_TAG ${FBJNI_GIT_TAG}
                SOURCE_DIR "${FBJNI_INSTALL_PREFIX}/fbjni-src"
                BINARY_DIR "${_fbjni_binary_dir}"
        )

        set(ENV{JAVA_HOME} "${JAVA_HOME}")
        set(FBJNI_SKIP_TESTS ON CACHE BOOL "Skip fbjni tests" FORCE)

        # 使用 Populate（而非 MakeAvailable）以便在 add_subdirectory 前打补丁
        FetchContent_Populate(fbjni)
        set(_fbjni_source_dir "${fbjni_SOURCE_DIR}")
        set(fbjni_DIR "${fbjni_SOURCE_DIR}" CACHE PATH ${_FBJNI_DIR_DOC} FORCE)

    else ()
        # 未找到且未启用自动安装
        st_l_info(IFbjni7)

    endif ()


    if (_fbjni_source_dir)
        # 打补丁：SHARED -> STATIC（静态链接进消费者 DLL，避免运行时依赖 fbjni.dll）
        # 移除 MSVC 下的 PDB 安装规则（静态库无链接器 PDB，否则配置阶段报错）
        st_l_info(IFbjni2)
        file(READ "${_fbjni_source_dir}/CMakeLists.txt" _fbjni_cmake)
        string(REPLACE
                "add_library(fbjni SHARED"
                "add_library(fbjni STATIC"
                _fbjni_cmake "${_fbjni_cmake}")
        string(REPLACE
                "install(FILES $<TARGET_PDB_FILE:fbjni> DESTINATION \${CMAKE_INSTALL_LIBDIR} OPTIONAL)"
                ""
                _fbjni_cmake "${_fbjni_cmake}")
        file(WRITE "${_fbjni_source_dir}/CMakeLists.txt" "${_fbjni_cmake}")

        add_subdirectory("${_fbjni_source_dir}" "${_fbjni_binary_dir}" EXCLUDE_FROM_ALL)

        if (TARGET fbjni)
            # 静态链接时，FbjniApi.h 在未定义 BUILDING_FBJNI 时使用 __declspec(dllimport)，
            # 这对静态库是错误的。将 BUILDING_FBJNI 暴露为 PUBLIC 定义。
            target_compile_definitions(fbjni PUBLIC BUILDING_FBJNI)
            st_l_info(IFbjni3)
        else ()
            st_l_warning(WFbjni3)
        endif ()
    endif ()

endif ()


# -------------------- 设置结果变量 --------------------
if (TARGET fbjni)
    if (NOT TARGET fbjni::fbjni)
        add_library(fbjni::fbjni ALIAS fbjni)
    endif ()

    get_target_property(fbjni_INCLUDE_DIRS fbjni INTERFACE_INCLUDE_DIRECTORIES)
    if (NOT fbjni_INCLUDE_DIRS)
        if (_fbjni_source_dir)
            set(fbjni_INCLUDE_DIRS "${_fbjni_source_dir}/cxx")
        elseif (fbjni_DIR)
            set(fbjni_INCLUDE_DIRS "${fbjni_DIR}/cxx")
        endif ()
    endif ()
    list(APPEND fbjni_INCLUDE_DIRS ${JNI_INCLUDE_DIRS})

    set(fbjni_LIBRARIES fbjni)
    set(fbjni_VERSION "${FBJNI_GIT_TAG}")

    target_link_libraries(fbjni PRIVATE JNI::JNI)

endif ()


# -------------------- 标准 FindPackage 处理 --------------------
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(fbjni
        REQUIRED_VARS fbjni_LIBRARIES fbjni_INCLUDE_DIRS
        VERSION_VAR fbjni_VERSION
        HANDLE_COMPONENTS
)


if (fbjni_FOUND)
    st_l_info_fmt(IFbjni4 ${fbjni_INCLUDE_DIRS})
    st_l_info_fmt(IFbjni5 ${fbjni_LIBRARIES})
    if (fbjni_VERSION)
        st_l_info_fmt(IFbjni6 ${fbjni_VERSION})
    endif ()
endif ()
