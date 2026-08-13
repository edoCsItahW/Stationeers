include(${CMAKE_CURRENT_LIST_DIR}/modules/Locale.cmake)

# 确保 Node 包已被查找
find_package(Node REQUIRED)

# 如果 Node 未找到，本模块无法工作（但 find_package REQUIRED 会终止）
# 这里可以添加保护
if(NOT Node_FOUND)
    return()
endif()

function(_get_napi NODE_EXE NAPI_INCLUDE NAPI_FOUND)
    execute_process(
            COMMAND ${NODE_EXE} -p "require('node-addon-api').include"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE _napi_raw
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
    )
    if(_napi_raw AND NOT _napi_raw MATCHES "Error")
        string(REPLACE "\"" "" _napi_raw "${_napi_raw}")
        file(TO_CMAKE_PATH "${_napi_raw}" _napi_path)
        if(EXISTS "${_napi_path}/napi.h")
            set(${NAPI_INCLUDE} "${_napi_path}" PARENT_SCOPE)
            set(${NAPI_FOUND} TRUE PARENT_SCOPE)
        else()
            set(${NAPI_FOUND} FALSE PARENT_SCOPE)
        endif()
    else()
        set(${NAPI_FOUND} FALSE PARENT_SCOPE)
    endif()

endfunction()


# -------------------- 内部函数：查找/安装 node-addon-api --------------------
function(_node_find_napi)
    # 如果已有缓存，直接返回
    if(DEFINED _NODE_NAPI_INCLUDE_DIR_CACHED AND NOT _NODE_NAPI_INCLUDE_DIR_CACHED STREQUAL "")
        set(Node_NAPI_INCLUDE_DIR "${_NODE_NAPI_INCLUDE_DIR_CACHED}" PARENT_SCOPE)
        if(_NODE_NAPI_INCLUDE_DIR_CACHED)
            set(Node_NAPI_FOUND TRUE PARENT_SCOPE)
        else()
            set(Node_NAPI_FOUND FALSE PARENT_SCOPE)
        endif()
        return()
    endif()

    set(_napi_include "")
    set(_found FALSE)

    _get_napi(${Node_EXECUTABLE} _napi_include _found)

    # 如果未找到且自动安装启用，则安装
    if(NOT _found AND NODE_AUTO_INSTALL)

        if(NOT DEFINED _NODE_PKG_MANAGER_CACHED)

            if(WIN32)
                find_program(_pnpm_exec NAMES pnpm.cmd pnpm.exe pnpm)
            else()
                find_program(_pnpm_exec pnpm)
            endif()
            if(_pnpm_exec)
                set(_NODE_PKG_MANAGER "${_pnpm_exec}")
                set(_NODE_PKG_INSTALL_CMD "add")
                set(_NODE_IS_PNPM TRUE)
            else()
                if(WIN32)
                    find_program(_npm_exec NAMES npm.cmd npm.exe npm)
                else()
                    find_program(_npm_exec npm)
                endif()
                if(_npm_exec)
                    set(_NODE_PKG_MANAGER "${_npm_exec}")
                    set(_NODE_PKG_INSTALL_CMD "install")
                    set(_NODE_IS_PNPM FALSE)
                else()
                    set(_NODE_PKG_MANAGER "")
                endif()
            endif()
        else()
            # 使用缓存的变量
        endif()

        if(_NODE_PKG_MANAGER)
            st_l_info(INode10)  # Installing node-addon-api
            set(_install_cmd ${_NODE_PKG_MANAGER} ${_NODE_PKG_INSTALL_CMD} ${_NODE_PKG_SAVE_CMD} "node-addon-api@latest")

            execute_process(
                    COMMAND ${_install_cmd}
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    RESULT_VARIABLE _result
                    ERROR_VARIABLE _error
                    OUTPUT_QUIET
            )
            if(NOT _result EQUAL 0)
                st_l_warning_fmt(WNode11 ${_error})

            else()
                _get_napi(${Node_EXECUTABLE} _napi_include _found)

            endif()
        else()
            st_l_warning(WNode12)  # No package manager to install node-addon-api

        endif()
    endif()

    st_localize(DNode25 _NODE_NAPI_INCLUDE_DIR_CACHED_DOC)

    # 缓存结果
    if(_found)
        set(_NODE_NAPI_INCLUDE_DIR_CACHED "${_napi_include}" CACHE INTERNAL ${_NODE_NAPI_INCLUDE_DIR_CACHED_DOC})
        set(Node_NAPI_INCLUDE_DIR "${_napi_include}" PARENT_SCOPE)
        set(Node_NAPI_FOUND TRUE PARENT_SCOPE)
    else()
        set(_NODE_NAPI_INCLUDE_DIR_CACHED "" CACHE INTERNAL ${_NODE_NAPI_INCLUDE_DIR_CACHED_DOC})
        set(Node_NAPI_INCLUDE_DIR "" PARENT_SCOPE)
        set(Node_NAPI_FOUND FALSE PARENT_SCOPE)
    endif()
endfunction()

# 调用函数
_node_find_napi()

# 如果找到，定义 Node::NAPI 目标
if(Node_NAPI_FOUND)
    if(NOT TARGET Node::NAPI)
        add_library(Node::NAPI INTERFACE IMPORTED)
        set_target_properties(Node::NAPI PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${Node_NAPI_INCLUDE_DIR}"
        )
    endif()
    st_l_info_fmt(INode16 ${Node_NAPI_INCLUDE_DIR})
else()
    st_l_info(INode17)  # Node-API not found
endif()