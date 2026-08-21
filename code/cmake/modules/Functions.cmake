include(${CMAKE_CURRENT_LIST_DIR}/Locale.cmake)

function(st_alias ALIAS_NAME TARGET)
    if (TARGET ${ALIAS_NAME})
        return()
    endif ()

    get_target_property(_imported ${TARGET} IMPORTED)
    if (_imported)
        set_target_properties(${TARGET} PROPERTIES IMPORTED_GLOBAL TRUE)
        add_library(${ALIAS_NAME} INTERFACE IMPORTED GLOBAL)
        set_target_properties(${ALIAS_NAME} PROPERTIES INTERFACE_LINK_LIBRARIES ${TARGET})

        # 传播 STATIONEERS_HASH 到别名，确保依赖该别名计算哈希时
        # 能获取到底层目标的哈希（INTERFACE IMPORTED 不会像 ALIAS 那样转发属性查询）
        get_target_property(_hash ${TARGET} STATIONEERS_HASH)
        if (_hash)
            set_target_properties(${ALIAS_NAME} PROPERTIES STATIONEERS_HASH "${_hash}")
        endif ()

        # 缓存命中时，复制缓存文件到构建输出路径
        get_target_property(_cache_path ${TARGET} STATIONEERS_CACHE_PATH)
        if (_cache_path)
            get_target_property(_prefix ${TARGET} PREFIX)
            get_target_property(_suffix ${TARGET} SUFFIX)
            get_target_property(_output_name ${TARGET} OUTPUT_NAME)

            if (NOT _output_name)
                set(_output_name ${TARGET})
            endif ()
            if (NOT _prefix)
                set(_prefix "")
            endif ()
            if (NOT _suffix)
                set(_suffix ${CMAKE_STATIC_LIBRARY_SUFFIX})
            endif ()

            set(_output_file_name "${_prefix}${_output_name}${_suffix}")

            if (CMAKE_CONFIGURATION_TYPES)
                foreach (_cfg ${CMAKE_CONFIGURATION_TYPES})
                    set(_output_dir "${CMAKE_CURRENT_BINARY_DIR}/${_cfg}")
                    file(MAKE_DIRECTORY "${_output_dir}")
                    execute_process(
                            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                            "${_cache_path}" "${_output_dir}/${_output_file_name}"
                    )
                endforeach ()
            else ()
                set(_output_dir "${CMAKE_CURRENT_BINARY_DIR}")
                execute_process(
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${_cache_path}" "${_output_dir}/${_output_file_name}"
                )
            endif ()
        endif ()
    else ()
        add_library(${ALIAS_NAME} ALIAS ${TARGET})
    endif ()
endfunction()

function(st_is_imported TARGET RESULT)
    get_target_property(_imported ${TARGET} IMPORTED)
    if (_imported)
        set(${RESULT} TRUE PARENT_SCOPE)
    else ()
        set(${RESULT} FALSE PARENT_SCOPE)
    endif ()
endfunction()

macro(st_l_options NAME MSG_ID VALUE)
    st_localize(${MSG_ID} _${NAME}_DOC)
    option(${NAME} "${_${NAME}_DOC}" ${VALUE})
endmacro()


function(st_source SRC SRC_DIR)

    if (EXISTS "${SRC_DIR}")

        file(GLOB_RECURSE _src_list CONFIGURE_DEPENDS
                "${SRC_DIR}/*.cpp"
        )
        set(${SRC} ${_src_list} PARENT_SCOPE)

    else ()

        set(${SRC} "" PARENT_SCOPE)
        st_l_error_fmt(EFunctions0 ${SRC_DIR})

    endif ()

endfunction()


function(_st_configure_target TARGET SOURCES INCLUDE_DIR)
    set(options)
    set(oneValueArgs SOURCES_MODE INCLUDE_MODE PCH_MODE)
    set(multiValueArgs PCH PRIVATE_LINK PUBLIC_LINK COMPILE_DEFINITIONS EXTRA_LIBS LINK_OPTIONS)

    cmake_parse_arguments(PARSE_ARGV 3 ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}")

    if (NOT DEFINED ARGS_SOURCES_MODE)
        set(ARGS_SOURCES_MODE PRIVATE)
    endif ()
    if (NOT DEFINED ARGS_INCLUDE_MODE)
        set(ARGS_INCLUDE_MODE PUBLIC)
    endif ()
    if (NOT DEFINED ARGS_PCH_MODE)
        set(ARGS_PCH_MODE PRIVATE)
    endif ()

    target_sources(${TARGET} ${ARGS_SOURCES_MODE} ${SOURCES})
    target_include_directories(${TARGET} ${ARGS_INCLUDE_MODE} ${INCLUDE_DIR})

    if (ARGS_PRIVATE_LINK OR ARGS_EXTRA_LIBS)
        target_link_libraries(${TARGET} PRIVATE ${ARGS_PRIVATE_LINK} ${ARGS_EXTRA_LIBS})
    elseif (ARGS_PUBLIC_LINK)
        target_link_libraries(${TARGET} PUBLIC ${ARGS_PUBLIC_LINK})
    endif ()

    if (DEFINED ARGS_PCH)
        target_precompile_headers(${TARGET} ${ARGS_PCH_MODE} ${ARGS_PCH})
    endif ()

    if (DEFINED ARGS_COMPILE_DEFINITIONS)
        target_compile_definitions(${TARGET} PRIVATE ${ARGS_COMPILE_DEFINITIONS})
    endif ()

    if (ARGS_LINK_OPTIONS)
        target_link_options(${TARGET} PRIVATE ${ARGS_LINK_OPTIONS})
    endif ()
endfunction()


function(st_library TARGET SOURCES INCLUDE_DIR)
    set(oneValueArgs LIBRARY_TYPE)
    cmake_parse_arguments(PARSE_ARGV 3 ST_LIB "" "${oneValueArgs}" "")

    if (NOT DEFINED ST_LIB_LIBRARY_TYPE)
        set(ST_LIB_LIBRARY_TYPE STATIC)
    endif ()

    add_library(${TARGET} ${ST_LIB_LIBRARY_TYPE} ${SOURCES})

    _st_configure_target(${TARGET} "${SOURCES}" "${INCLUDE_DIR}" ${ST_LIB_UNPARSED_ARGUMENTS})
endfunction()


function(st_executable TARGET SOURCES INCLUDE_DIR)
    add_executable(${TARGET} ${SOURCES})

    _st_configure_target(${TARGET} "${SOURCES}" "${INCLUDE_DIR}" ${ARGN})
endfunction()


macro(resolve_feature_tree ROOT)
    _dfs(${ROOT} OFF "")

    get_cmake_property(_vars CACHE_VARIABLES)

    foreach (_var ${_vars})
        if (_var MATCHES "ANY_.*" OR _var MATCHES "^${ROOT}_.*")
            set(${_var} $CACHE{${_var}})
        endif ()

    endforeach ()

endmacro()


function(_dfs PARENT FLAG PATH_LIST)
    list(APPEND PATH_LIST "${PARENT}")

    string(REPLACE ";" "_" prefix "${PATH_LIST}")

    if (DEFINED ${prefix})
        set(result ${${prefix}})
    else ()
        set(result ${FLAG})
    endif ()

    set(any_result ${result})

    if (DEFINED ${PARENT}_CHILDREN)
        set(children ${${PARENT}_CHILDREN})

        foreach (child ${children})
            _dfs(${child} ${result} "${PATH_LIST}")

            set(child_var "${prefix}_${child}")
            if (${child_var})
                set(any_result ON)
            endif ()

        endforeach ()

    endif ()

    if (any_result)
        set(${prefix} ON CACHE INTERNAL "" FORCE)
    else ()
        set(${prefix} OFF CACHE INTERNAL "" FORCE)
    endif ()

    if (any_result OR ANY_${PARENT})
        set(ANY_${PARENT} ON CACHE INTERNAL "" FORCE)
    else ()
        set(ANY_${PARENT} OFF CACHE INTERNAL "" FORCE)
    endif ()

endfunction()


function(_enable_binary_target CACHE_FLAG CACHE_DIR TARGET SOURCES INCLUDE_DIR TARGET_KIND)
    set(options)
    set(oneValueArgs LIBRARY_TYPE SOURCES_MODE INCLUDE_MODE PCH_MODE)
    set(multiValueArgs PCH PRIVATE_LINK PUBLIC_LINK COMPILE_DEFINITIONS LINK_OPTIONS)

    cmake_parse_arguments(PARSE_ARGV 6 ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}")

    if (NOT CACHE_FLAG)
        if (TARGET_KIND STREQUAL "LIBRARY")
            st_library(${TARGET} "${SOURCES}" "${INCLUDE_DIR}" ${ARGN})
        else()
            st_executable(${TARGET} "${SOURCES}" "${INCLUDE_DIR}" ${ARGN})
        endif()
        return()
    endif ()

    # ---------- 设置默认值 ----------
    if (NOT DEFINED ARGS_LIBRARY_TYPE)
        set(ARGS_LIBRARY_TYPE STATIC)
    endif ()
    if (NOT DEFINED ARGS_SOURCES_MODE)
        set(ARGS_SOURCES_MODE PRIVATE)
    endif ()
    if (NOT DEFINED ARGS_INCLUDE_MODE)
        set(ARGS_INCLUDE_MODE PUBLIC)
    endif ()
    if (NOT DEFINED ARGS_PCH_MODE)
        set(ARGS_PCH_MODE PRIVATE)
    endif ()

    # ----- 启用缓存 -----
    file(MAKE_DIRECTORY "${CACHE_DIR}")

    # 收集所有源文件及头文件（用于计算哈希）
    if (NOT ${INCLUDE_DIR} STREQUAL "")
        file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "${INCLUDE_DIR}/**/*.h" "${INCLUDE_DIR}/**/*.hpp" "${INCLUDE_DIR}/**/*.inl")
        list(APPEND FILES ${SOURCES} ${HEADERS})
    else ()
        set(FILES "${SOURCES}")
    endif ()

    # 计算哈希（包含编译器和编译选项）
    set(FLAGS
            ${CMAKE_SYSTEM_NAME}
            ${CMAKE_CXX_COMPILER_ID}
            ${CMAKE_CXX_COMPILER_VERSION}
            ${CMAKE_CXX_FLAGS}
            ${CMAKE_EXE_LINKER_FLAGS}
    )
    hash_files("${FILES}" FILES_HASH)
    string(APPEND FILES_HASH "${FLAGS}")

    set(DEP_HASH "")
    foreach (lib IN LISTS ARGS_PRIVATE_LINK ARGS_PUBLIC_LINK)
        if (TARGET ${lib})
            get_target_property(_hash ${lib} STATIONEERS_HASH)

            if (_hash)
                string(APPEND DEP_HASH "${_hash}")
            endif ()
        endif ()
    endforeach ()

    string(APPEND FILES_HASH "${DEP_HASH}")
    string(MD5 HASH "${FILES_HASH}")

    # 构造缓存文件名
    if (TARGET_KIND STREQUAL "LIBRARY")
        if (DEFINED ARGS_LIBRARY_TYPE AND ARGS_LIBRARY_TYPE STREQUAL "SHARED")
            set(LIB_PREFIX ${CMAKE_SHARED_LIBRARY_PREFIX})
            set(LIB_SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX})
        else ()
            set(LIB_PREFIX ${CMAKE_STATIC_LIBRARY_PREFIX})
            set(LIB_SUFFIX ${CMAKE_STATIC_LIBRARY_SUFFIX})
        endif ()
        set(OUTPUT_NAME "${LIB_PREFIX}${TARGET}_${HASH}${LIB_SUFFIX}")
    else ()  # EXECUTABLE
        set(OUTPUT_NAME "${CMAKE_EXECUTABLE_PREFIX}${TARGET}_${HASH}${CMAKE_EXECUTABLE_SUFFIX}")
    endif ()
    set(CACHE_PATH "${CACHE_DIR}/${OUTPUT_NAME}")

    # 构造导入库缓存路径（仅 Windows SHARED 库：MSVC/MinGW 生成 .lib/.dll.a 导入库）
    if (TARGET_KIND STREQUAL "LIBRARY" AND DEFINED ARGS_LIBRARY_TYPE AND ARGS_LIBRARY_TYPE STREQUAL "SHARED"
            AND CMAKE_IMPORT_LIBRARY_SUFFIX)
        set(CACHE_IMPLIB_PATH
                "${CACHE_DIR}/${CMAKE_IMPORT_LIBRARY_PREFIX}${TARGET}_${HASH}${CMAKE_IMPORT_LIBRARY_SUFFIX}")
    endif ()

    # 检查缓存是否存在
    if (EXISTS "${CACHE_PATH}")
        set(CACHE_HIT TRUE)
        st_l_info_fmt(IFunctions3 ${TARGET} ${HASH})
    else ()
        set(CACHE_HIT FALSE)
        st_l_info_fmt(IFunctions4 ${TARGET} ${HASH})
    endif ()

    if (CACHE_HIT)
        # ---------- 缓存命中：创建 IMPORTED 目标（不构建源码） ----------
        if (TARGET_KIND STREQUAL "LIBRARY")
            # 创建 IMPORTED 库
            if (DEFINED ARGS_LIBRARY_TYPE)
                add_library(${TARGET} ${ARGS_LIBRARY_TYPE} IMPORTED)
            else ()
                add_library(${TARGET} STATIC IMPORTED)
            endif ()
            set_target_properties(${TARGET} PROPERTIES
                    IMPORTED_LOCATION "${CACHE_PATH}"
                    INTERFACE_INCLUDE_DIRECTORIES "${INCLUDE_DIR}"
                    STATIONEERS_CACHE_PATH "${CACHE_PATH}"
            )
            # 设置导入库路径（Windows SHARED 库需要 IMPORTED_IMPLIB 供其他目标链接）
            if (DEFINED CACHE_IMPLIB_PATH AND EXISTS "${CACHE_IMPLIB_PATH}")
                set_target_properties(${TARGET} PROPERTIES IMPORTED_IMPLIB "${CACHE_IMPLIB_PATH}")
            endif ()
            # 设置接口链接库和编译定义
            if (ARGS_PUBLIC_LINK)
                set_target_properties(${TARGET} PROPERTIES INTERFACE_LINK_LIBRARIES "${ARGS_PUBLIC_LINK}")
            endif ()
            if (ARGS_COMPILE_DEFINITIONS)
                set_target_properties(${TARGET} PROPERTIES INTERFACE_COMPILE_DEFINITIONS "${ARGS_COMPILE_DEFINITIONS}")
            endif ()
            # 处理额外的依赖（通过 UNPARSED_ARGUMENTS 传入的）
            if (ARGS_UNPARSED_ARGUMENTS)
                get_target_property(_existing_link_libs ${TARGET} INTERFACE_LINK_LIBRARIES)
                if (_existing_link_libs)
                    set(_interface_libs ${_existing_link_libs} ${ARGS_UNPARSED_ARGUMENTS})
                else ()
                    set(_interface_libs ${ARGS_UNPARSED_ARGUMENTS})
                endif ()
                set_target_properties(${TARGET} PROPERTIES INTERFACE_LINK_LIBRARIES "${_interface_libs}")
            endif ()

            # 复制缓存文件到构建输出目录（多配置支持）
            if (DEFINED ARGS_LIBRARY_TYPE AND ARGS_LIBRARY_TYPE STREQUAL "SHARED")
                set(_lib_prefix ${CMAKE_SHARED_LIBRARY_PREFIX})
                set(_lib_suffix ${CMAKE_SHARED_LIBRARY_SUFFIX})
            else ()
                set(_lib_prefix ${CMAKE_STATIC_LIBRARY_PREFIX})
                set(_lib_suffix ${CMAKE_STATIC_LIBRARY_SUFFIX})
            endif ()
            set(_out_name "${_lib_prefix}${TARGET}${_lib_suffix}")
            # 导入库输出文件名（Windows SHARED 库）
            if (DEFINED CACHE_IMPLIB_PATH)
                set(_implib_out_name "${CMAKE_IMPORT_LIBRARY_PREFIX}${TARGET}${CMAKE_IMPORT_LIBRARY_SUFFIX}")
            endif ()
            if (CMAKE_CONFIGURATION_TYPES)
                foreach (_cfg ${CMAKE_CONFIGURATION_TYPES})
                    set(_out_dir "${CMAKE_CURRENT_BINARY_DIR}/${_cfg}")
                    file(MAKE_DIRECTORY "${_out_dir}")
                    execute_process(
                            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                            "${CACHE_PATH}" "${_out_dir}/${_out_name}"
                    )
                    if (DEFINED CACHE_IMPLIB_PATH AND EXISTS "${CACHE_IMPLIB_PATH}")
                        execute_process(
                                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                                "${CACHE_IMPLIB_PATH}" "${_out_dir}/${_implib_out_name}"
                        )
                    endif ()
                endforeach ()
            else ()
                set(_out_dir "${CMAKE_CURRENT_BINARY_DIR}")
                execute_process(
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${CACHE_PATH}" "${_out_dir}/${_out_name}"
                )
                if (DEFINED CACHE_IMPLIB_PATH AND EXISTS "${CACHE_IMPLIB_PATH}")
                    execute_process(
                            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                            "${CACHE_IMPLIB_PATH}" "${_out_dir}/${_implib_out_name}"
                    )
                endif ()
            endif ()

        else ()  # EXECUTABLE
            # 创建 dummy 可执行文件（用于依赖追踪）
            set(_dummy_src "${CMAKE_BINARY_DIR}/_st_cache_${TARGET}.cpp")
            file(WRITE "${_dummy_src}" "int main(){return 0;}")
            add_executable(${TARGET} "${_dummy_src}")
            set_target_properties(${TARGET} PROPERTIES
                    STATIONEERS_CACHE_PATH "${CACHE_PATH}"
            )
            # 添加 POST_BUILD 命令复制缓存文件到实际可执行文件位置
            add_custom_command(TARGET ${TARGET} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${CACHE_PATH}" "$<TARGET_FILE:${TARGET}>"
                    VERBATIM
            )
        endif ()

    else ()
        # ---------- 缓存未命中：构建源码并生成缓存 ----------
        if (TARGET_KIND STREQUAL "LIBRARY")
            st_library(${TARGET} "${SOURCES}" "${INCLUDE_DIR}"
                    LIBRARY_TYPE ${ARGS_LIBRARY_TYPE}
                    SOURCES_MODE ${ARGS_SOURCES_MODE}
                    INCLUDE_MODE ${ARGS_INCLUDE_MODE}
                    PCH_MODE ${ARGS_PCH_MODE}
                    PCH ${ARGS_PCH}
                    PRIVATE_LINK ${ARGS_PRIVATE_LINK}
                    PUBLIC_LINK ${ARGS_PUBLIC_LINK}
                    COMPILE_DEFINITIONS ${ARGS_COMPILE_DEFINITIONS}
                    LINK_OPTIONS ${ARGS_LINK_OPTIONS}
                    ${ARGS_UNPARSED_ARGUMENTS}
            )
        else ()
            st_executable(${TARGET} "${SOURCES}" "${INCLUDE_DIR}"
                    SOURCES_MODE ${ARGS_SOURCES_MODE}
                    INCLUDE_MODE ${ARGS_INCLUDE_MODE}
                    PCH_MODE ${ARGS_PCH_MODE}
                    PCH ${ARGS_PCH}
                    PRIVATE_LINK ${ARGS_PRIVATE_LINK}
                    PUBLIC_LINK ${ARGS_PUBLIC_LINK}
                    COMPILE_DEFINITIONS ${ARGS_COMPILE_DEFINITIONS}
                    LINK_OPTIONS ${ARGS_LINK_OPTIONS}
                    ${ARGS_UNPARSED_ARGUMENTS}
            )
        endif ()

        # 构建后复制产物到缓存目录
        st_localize_fmt(DFunctions5 _COMMENT ${TARGET} ${CACHE_PATH})
        if (DEFINED CACHE_IMPLIB_PATH)
            # Windows SHARED 库：同时复制 .dll 和 .lib（导入库）到缓存
            add_custom_command(TARGET ${TARGET} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${TARGET}> "${CACHE_PATH}"
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_LINKER_FILE:${TARGET}> "${CACHE_IMPLIB_PATH}"
                    COMMENT "${_COMMENT}"
                    VERBATIM
            )
        else ()
            add_custom_command(TARGET ${TARGET} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${TARGET}> "${CACHE_PATH}"
                    COMMENT "${_COMMENT}"
                    VERBATIM
            )
        endif ()
    endif ()

    # 让 CMake 在源码改动时重新配置
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${FILES}")

    # 设置目标哈希，使得依赖该目标的依赖项发生变化
    set_target_properties(${TARGET} PROPERTIES STATIONEERS_HASH "${HASH}")

endfunction()


function(enable_library_cache CACHE_FLAG CACHE_DIR TARGET SOURCES INCLUDE_DIR)
    _enable_binary_target(${CACHE_FLAG} ${CACHE_DIR} ${TARGET} "${SOURCES}" "${INCLUDE_DIR}" "LIBRARY" ${ARGN})
endfunction()

function(enable_executable_cache CACHE_FLAG CACHE_DIR TARGET SOURCES INCLUDE_DIR)
    _enable_binary_target(${CACHE_FLAG} ${CACHE_DIR} ${TARGET} "${SOURCES}" "${INCLUDE_DIR}" "EXECUTABLE" ${ARGN})
endfunction()


function(hash_files FILES HASH)
    set(COMBINED_HASH "")

    foreach (F ${FILES})
        if (EXISTS ${F})
            file(MD5 ${F} FILE_HASH)
            string(APPEND COMBINED_HASH "${FILE_HASH}")

        else ()
            st_l_error_fmt(EFunctions6 ${F})

        endif ()

    endforeach ()

    string(MD5 RESULT "${COMBINED_HASH}")
    set(${HASH} "${RESULT}" PARENT_SCOPE)

endfunction()
