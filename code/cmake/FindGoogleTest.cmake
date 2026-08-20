include(${CMAKE_CURRENT_LIST_DIR}/modules/Locale.cmake)


# -------------------- 选项 --------------------
st_localize(DGoogleTest0 _AUTO_INSTALL_DOC)
option(GOOGLETEST_AUTO_INSTALL ${_AUTO_INSTALL_DOC} ON)

st_localize(DGoogleTest1 _GIT_TAG_DOC)
set(GOOGLETEST_GIT_TAG "v1.17.0" CACHE STRING ${_GIT_TAG_DOC})

st_localize(DGoogleTest2 _INSTALL_PREFIX_DOC)
set(GOOGLETEST_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/third-party" CACHE PATH ${_INSTALL_PREFIX_DOC})

st_localize(DGoogleTest3 _DIR_DOC)


# -------------------- 查找现有 GoogleTest 源码目录 --------------------
# GoogleTest 通过 add_subdirectory 构建，不生成 CMake config 包，
# 因此不能用 find_package(GoogleTest CONFIG) 查找。这里检测 googletest-src
# 目录是否已存在，避免重复下载。
if (NOT GoogleTest_DIR AND NOT DEFINED ENV{GOOGLETEST_DIR})
    set(_gtest_search_paths
            "${CMAKE_SOURCE_DIR}/third-party"
            "${GOOGLETEST_INSTALL_PREFIX}"
            "/usr/local"
            "/usr"
    )

    foreach (_path ${_gtest_search_paths})
        if (EXISTS "${_path}/googletest-src/CMakeLists.txt")
            set(GoogleTest_DIR "${_path}/googletest-src" CACHE PATH ${_DIR_DOC})
            break()
        endif ()
    endforeach ()

endif ()

# 检查 GOOGLETEST_DIR 环境变量
if (NOT GoogleTest_DIR AND DEFINED ENV{GOOGLETEST_DIR})
    if (EXISTS "$ENV{GOOGLETEST_DIR}/CMakeLists.txt")
        set(GoogleTest_DIR "$ENV{GOOGLETEST_DIR}" CACHE PATH ${_DIR_DOC})
    endif ()
endif ()


# -------------------- 构建或复用 GoogleTest --------------------
set(_gtest_source_dir "")
set(_gtest_binary_dir "${CMAKE_BINARY_DIR}/dependencies/googletest-build")

if (NOT TARGET gtest)
    if (GoogleTest_DIR AND EXISTS "${GoogleTest_DIR}/CMakeLists.txt")
        # 源码已存在——跳过下载（检测以避免重复下载）
        st_l_info_fmt(IGoogleTest0 ${GoogleTest_DIR})
        set(_gtest_source_dir "${GoogleTest_DIR}")

    elseif (GOOGLETEST_AUTO_INSTALL)
        st_l_info_fmt(IGoogleTest1 ${GOOGLETEST_GIT_TAG})

        include(FetchContent)
        FetchContent_Declare(googletest
                GIT_REPOSITORY https://github.com/google/googletest.git
                GIT_TAG ${GOOGLETEST_GIT_TAG}
                SOURCE_DIR "${GOOGLETEST_INSTALL_PREFIX}/googletest-src"
                BINARY_DIR "${_gtest_binary_dir}"
        )

        # 使用 Populate（而非 MakeAvailable）以直接 add_subdirectory，
        # 避免 FetchContent 在源码已存在时重复 update。
        FetchContent_Populate(googletest)
        set(_gtest_source_dir "${googletest_SOURCE_DIR}")
        set(GoogleTest_DIR "${googletest_SOURCE_DIR}" CACHE PATH ${_DIR_DOC} FORCE)

    else ()
        # 未找到且未启用自动安装
        st_l_info(IGoogleTest3)

    endif ()


    if (_gtest_source_dir)
        add_subdirectory("${_gtest_source_dir}" "${_gtest_binary_dir}" EXCLUDE_FROM_ALL)

        if (TARGET gtest)
            st_l_info(IGoogleTest2)
        else ()
            st_l_warning(WGoogleTest0)
        endif ()
    endif ()

endif ()


# -------------------- 设置结果变量 --------------------
if (TARGET gtest)
    # GoogleTest 自身已提供 GTest::gtest / GTest::gtest_main 命名空间别名（1.10+），
    # 这里仅兜底确保别名存在。
    if (NOT TARGET GTest::gtest)
        add_library(GTest::gtest ALIAS gtest)
    endif ()
    if (NOT TARGET GTest::gtest_main)
        add_library(GTest::gtest_main ALIAS gtest_main)
    endif ()

    get_target_property(GoogleTest_INCLUDE_DIRS gtest INTERFACE_INCLUDE_DIRECTORIES)
    if (NOT GoogleTest_INCLUDE_DIRS)
        if (_gtest_source_dir)
            set(GoogleTest_INCLUDE_DIRS "${_gtest_source_dir}/googletest/include")
        elseif (GoogleTest_DIR)
            set(GoogleTest_INCLUDE_DIRS "${GoogleTest_DIR}/googletest/include")
        endif ()
    endif ()

    set(GoogleTest_LIBRARIES gtest gtest_main)
    set(GoogleTest_VERSION "${GOOGLETEST_GIT_TAG}")
endif ()


# -------------------- 标准 FindPackage 处理 --------------------
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GoogleTest
        REQUIRED_VARS GoogleTest_LIBRARIES GoogleTest_INCLUDE_DIRS
        VERSION_VAR GoogleTest_VERSION
        HANDLE_COMPONENTS
)


if (GoogleTest_FOUND)
    st_l_info_fmt(IGoogleTest4 ${GoogleTest_INCLUDE_DIRS})
    st_l_info_fmt(IGoogleTest5 ${GoogleTest_LIBRARIES})
    if (GoogleTest_VERSION)
        st_l_info_fmt(IGoogleTest6 ${GoogleTest_VERSION})
    endif ()
endif ()
