include(${CMAKE_CURRENT_LIST_DIR}/modules/Locale.cmake)


function(_node_find_package_manager)
    if(NOT DEFINED _NODE_PKG_MANAGER_CACHED)
        st_localize(DNode23 _NODE_PKG_MANAGER_LOOKUP_DOC)
        set(_NODE_PKG_MANAGER_CACHED TRUE CACHE INTERNAL ${_NODE_PKG_MANAGER_LOOKUP_DOC})

        if(WIN32)
            find_program(_pnpm_exec NAMES pnpm.cmd pnpm.exe pnpm)
        else()
            find_program(_pnpm_exec pnpm)
        endif()

        st_localize(DNode18 _NODE_PKG_MANAGER_DOC)
        st_localize(DNode19 _NODE_PKG_INSTALL_DOC)
        st_localize(DNode20 _NODE_PKG_EXEC_DOC)
        st_localize(DNode21 _NODE_PKG_DLX_DOC)
        st_localize(DNode22 _NODE_PKG_IS_PNPM_DOC)
        st_localize(DNode24 _NODE_PKG_SAVE_DOC)

        if(_pnpm_exec)
            set(_NODE_PKG_MANAGER "${_pnpm_exec}" CACHE INTERNAL ${_NODE_PKG_MANAGER_DOC})
            set(_NODE_PKG_INSTALL_CMD "add" CACHE INTERNAL ${_NODE_PKG_INSTALL_DOC})
            set(_NODE_PKG_EXEC_CMD "exec" CACHE INTERNAL ${_NODE_PKG_EXEC_DOC})
            set(_NODE_PKG_DLX_CMD "dlx" CACHE INTERNAL ${_NODE_PKG_DLX_DOC})
            set(_NODE_IS_PNPM TRUE CACHE INTERNAL ${_NODE_PKG_IS_PNPM_DOC})
            set(_NODE_PKG_SAVE_CMD "--save-dev" CACHE INTERNAL ${_NODE_PKG_SAVE_DOC})
        else()
            if(WIN32)
                find_program(_npm_exec NAMES npm.cmd npm.exe npm)
            else()
                find_program(_npm_exec npm)
            endif()
            if(_npm_exec)
                set(_NODE_PKG_MANAGER "${_npm_exec}" CACHE INTERNAL ${_NODE_PKG_MANAGER_DOC})
                set(_NODE_PKG_INSTALL_CMD "install" CACHE INTERNAL ${_NODE_PKG_INSTALL_DOC})
                set(_NODE_PKG_EXEC_CMD "exec" CACHE INTERNAL ${_NODE_PKG_EXEC_DOC})
                set(_NODE_PKG_DLX_CMD "npx" CACHE INTERNAL ${_NODE_PKG_DLX_DOC})
                set(_NODE_IS_PNPM FALSE CACHE INTERNAL ${_NODE_PKG_IS_PNPM_DOC})
                set(_NODE_PKG_SAVE_CMD "--no-save" CACHE INTERNAL ${_NODE_PKG_SAVE_DOC})
            else()
                set(_NODE_PKG_MANAGER "" CACHE INTERNAL ${_NODE_PKG_MANAGER_DOC})
                set(_NODE_PKG_INSTALL_CMD "" CACHE INTERNAL "")
                set(_NODE_PKG_EXEC_CMD "" CACHE INTERNAL "")
                set(_NODE_PKG_DLX_CMD "" CACHE INTERNAL "")
                set(_NODE_IS_PNPM FALSE CACHE INTERNAL "")
                set(_NODE_PKG_SAVE_CMD "" CACHE INTERNAL "")
            endif()
        endif()
    endif()
endfunction()


function(_node_install_headers target_dir node_version)
    _node_find_package_manager()
    if(NOT _NODE_PKG_MANAGER)
        st_l_warning(WNode5)  # No package manager found, cannot install headers
        return()
    endif()

    set(_header_path "${target_dir}/include/node/node.h")
    if(EXISTS "${_header_path}")
        return()
    endif()

    st_l_info(INode4)  # Installing node headers

    if(_NODE_IS_PNPM)
        set(_install_cmd ${_NODE_PKG_MANAGER} ${_NODE_PKG_DLX_CMD} node-gyp install)
    else()
        # For npm, try npx first, fallback to exec
        set(_install_cmd ${_NODE_PKG_MANAGER} ${_NODE_PKG_DLX_CMD} node-gyp install)
        # If npx is not available (or the current npm is npx itself), use exec
        if(NOT EXISTS "${_NODE_PKG_MANAGER}-npx" AND NOT _NODE_PKG_MANAGER STREQUAL "npx")
            set(_install_cmd ${_NODE_PKG_MANAGER} ${_NODE_PKG_EXEC_CMD} node-gyp install)
        endif()
    endif()

    execute_process(
            COMMAND ${_install_cmd}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            RESULT_VARIABLE _result
            ERROR_VARIABLE _error
            OUTPUT_QUIET
    )
    if(NOT _result EQUAL 0)
        st_l_warning_fmt(WNode6 ${_error})
    else()
        st_l_info(INode7)
    endif()
endfunction()

# -------------------- 主查找流程 --------------------
# 1. 查找 Node 可执行文件
st_localize(DNode0 _NODE_EXE)
find_program(Node_EXECUTABLE NAMES node nodejs PATHS ENV PATH DOC ${_NODE_EXE} REQUIRED)
if(NOT Node_EXECUTABLE)
    set(Node_FOUND FALSE)
    st_localize(DNode1 Node_FOUND_REASON)
    return()
endif()

# 2. 获取版本
execute_process(
        COMMAND ${Node_EXECUTABLE} -p "process.version.slice(1)"
        OUTPUT_VARIABLE Node_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
)
if(NOT Node_VERSION)
    set(Node_FOUND FALSE)
    st_localize(DNode2 Node_VERSION_REASON)
    return()
endif()

# 3. 确定搜索根路径
# 3a. 缓存根
if(DEFINED ENV{NODE_GYP_CACHE_ROOT})
    set(_node_gyp_cache_root "$ENV{NODE_GYP_CACHE_ROOT}")
elseif(WIN32)
    set(_node_gyp_cache_root "$ENV{LOCALAPPDATA}/node-gyp/Cache")
else()
    set(_node_gyp_cache_root "$ENV{HOME}/.cache/node-gyp")
endif()

# 3b. 用户指定的 NODE_ROOT
if(DEFINED NODE_ROOT)
    set(_node_search_root "${NODE_ROOT}")
elseif(DEFINED ENV{NODE_ROOT})
    set(_node_search_root "$ENV{NODE_ROOT}")
else()
    set(_node_search_root "${_node_gyp_cache_root}/${Node_VERSION}")
endif()

# 4. 自动安装（如果开启且未指定 NODE_ROOT）
st_localize(DNode3 _NODE_AUTO_INSTALL_DOC)
option(NODE_AUTO_INSTALL ${_NODE_AUTO_INSTALL_DOC} ON)

if(NODE_AUTO_INSTALL AND NOT (DEFINED ENV{NODE_ROOT} OR DEFINED NODE_ROOT))
    _node_install_headers("${_node_search_root}" "${Node_VERSION}")
endif()

# 5. 查找头文件
find_path(Node_INCLUDE_DIR
        NAMES node.h
        PATHS "${_node_search_root}/include/node"
        "${_node_search_root}/include"
        "${_node_search_root}"
        NO_DEFAULT_PATH
)
if(NOT Node_INCLUDE_DIR)
    set(Node_FOUND FALSE)
    st_localize(DNode8 Node_INCLUDE_DIR_REASON)
    return()
endif()

# 6. 查找库（仅 Windows）
if(WIN32)
    set(_node_lib_suffixes x64 x86 Release Debug)
    find_library(Node_LIBRARY NAMES node.lib
            PATHS "${_node_search_root}"
            "${_node_search_root}/x64"
            "${_node_search_root}/x86"
            "${_node_search_root}/Release"
            "${_node_search_root}/Debug"
            PATH_SUFFIXES ${_node_lib_suffixes}
            NO_DEFAULT_PATH
    )
    if(NOT Node_LIBRARY)
        find_library(Node_LIBRARY NAMES node.lib)  # fallback to default paths
    endif()
    if(NOT Node_LIBRARY)
        set(Node_FOUND FALSE)
        st_localize(DNode9 Node_LIBRARY_REASON)
        return()
    endif()
else()
    set(Node_LIBRARY "")
endif()

# 7. 设置输出变量
set(Node_INCLUDE_DIRS "${Node_INCLUDE_DIR}")
set(Node_LIBRARIES "${Node_LIBRARY}")

# 8. 标准包处理
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Node
        REQUIRED_VARS Node_INCLUDE_DIR
        VERSION_VAR Node_VERSION
)

if(NOT Node_FOUND)
    if(Node_FOUND_REASON)
        st_info(${Node_FOUND_REASON})
    endif()
    return()
endif()

# 9. 定义 Node::Node 目标
if(NOT TARGET Node::Node)
    if(WIN32 AND Node_LIBRARY)
        add_library(Node::Node UNKNOWN IMPORTED)
        set_target_properties(Node::Node PROPERTIES
                IMPORTED_LOCATION "${Node_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${Node_INCLUDE_DIR}"
        )
    else()
        add_library(Node::Node INTERFACE IMPORTED)
        set_target_properties(Node::Node PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${Node_INCLUDE_DIR}"
        )
    endif()
endif()

# 10. 信息输出
st_l_info_fmt(INode13 ${Node_VERSION})
st_l_info_fmt(INode14 ${Node_INCLUDE_DIRS})
if(WIN32)
    st_l_info_fmt(INode15 ${Node_LIBRARIES})
endif()