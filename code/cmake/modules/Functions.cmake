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

            foreach (_cfg ${CMAKE_CONFIGURATION_TYPES})
                set(_output_dir "${CMAKE_CURRENT_BINARY_DIR}/${_cfg}")
                file(MAKE_DIRECTORY "${_output_dir}")
                execute_process(
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${_cache_path}" "${_output_dir}/${_output_file_name}"
                )
            endforeach ()
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


function(st_library TARGET SOURCES INCLUDE_DIR)
    set(options)
    set(oneValueArgs
            LIBRARY_TYPE
            SOURCES_MODE
            INCLUDE_MODE
            PCH_MODE
    )
    set(multiValueArgs
            PCH
            PRIVATE_LINK
            PUBLIC_LINK
            COMPILE_DEFINITIONS
    )

    cmake_parse_arguments(ST_LIB
            "${options}"
            "${oneValueArgs}"
            "${multiValueArgs}"
            ${ARGN}
    )

    if (NOT DEFINED ST_LIB_PCH_MODE)
        set(ST_LIB_PCH_MODE PRIVATE)
    endif ()

    if (NOT DEFINED ST_LIB_LIBRARY_TYPE)
        set(ST_LIB_LIBRARY_TYPE STATIC)
    endif ()

    if (NOT DEFINED ST_LIB_SOURCES_MODE)
        set(ST_LIB_SOURCES_MODE PRIVATE)
    endif ()

    if (NOT DEFINED ST_LIB_INCLUDE_MODE)
        set(ST_LIB_INCLUDE_MODE PUBLIC)
    endif ()

    set(extra_libs ${ST_LIB_UNPARSED_ARGUMENTS})

    add_library(${TARGET} ${ST_LIB_LIBRARY_TYPE} ${SOURCES})

    TARGET_sources(${TARGET} ${ST_LIB_SOURCES_MODE} ${SOURCES})

    target_include_directories(${TARGET} ${ST_LIB_INCLUDE_MODE} ${INCLUDE_DIR})

    list(LENGTH ST_LIB_PRIVATE_LINK private_len)
    list(LENGTH extra_libs extra_len)
    math(EXPR private_total "${private_len} + ${extra_len}")

    if (private_total GREATER 0)
        set(all_private_libs ${ST_LIB_PRIVATE_LINK} ${extra_libs})
        target_link_libraries(${TARGET} PRIVATE ${all_private_libs})

    else ()
        list(LENGTH ST_LIB_PUBLIC_LINK public_len)
        if (public_len GREATER 0)
            target_link_libraries(${TARGET} PUBLIC ${ST_LIB_PUBLIC_LINK})
        endif ()

    endif ()

    if (DEFINED ST_LIB_PCH)
        target_precompile_headers(${TARGET} PUBLIC ${ST_LIB_PCH})

    endif ()

    if (ST_LIB_COMPILE_DEFINITIONS)
        target_compile_definitions(${TARGET} PRIVATE ${ST_LIB_COMPILE_DEFINITIONS})
    endif ()

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


function(enable_binary_cache CACHE_FLAG CACHE_DIR TARGET SOURCES INCLUDE_DIR)
    set(options)
    set(oneValueArgs LIBRARY_TYPE SOURCES_MODE INCLUDE_MODE PCH_MODE)
    set(multiValueArgs PCH PRIVATE_LINK PUBLIC_LINK COMPILE_DEFINITIONS)

    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT CACHE_FLAG)
        st_library(${TARGET} "${SOURCES}" "${INCLUDE_DIR}" ${ARGN})
        return()

    endif ()

    file(MAKE_DIRECTORY "${CACHE_DIR}")

    file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "${INCLUDE_DIR}/**/*.h" "${INCLUDE_DIR}/**/*.hpp")
    list(APPEND FILES ${SOURCES} ${HEADERS})

    set(FLAGS
            ${CMAKE_SYSTEM_NAME}           # 系统名称
            ${CMAKE_CXX_COMPILER_ID}       # 编译器名称
            ${CMAKE_CXX_COMPILER_VERSION}  # 编译器版本
            ${CMAKE_CXX_FLAGS}             # 编译器选项
            ${CMAKE_EXE_LINKER_FLAGS}      # 链接器选项
    )

    hash_files("${FILES}" FILES_HASH)

    string(APPEND FILES_HASH "${FLAGS}")
    string(MD5 HASH "${FILES_HASH}")

    if (DEFINED ARGS_LIBRARY_TYPE AND ARGS_LIBRARY_TYPE STREQUAL "SHARED")
        set(LIB_PREFIX ${CMAKE_SHARED_LIBRARY_PREFIX})
        set(LIB_SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX})
    else ()
        set(LIB_PREFIX ${CMAKE_STATIC_LIBRARY_PREFIX})
        set(LIB_SUFFIX ${CMAKE_STATIC_LIBRARY_SUFFIX})
    endif ()

    set(LIB_NAME "${LIB_PREFIX}${TARGET}_${HASH}${LIB_SUFFIX}")
    set(LIB_PATH "${CACHE_DIR}/${LIB_NAME}")

    if (EXISTS "${LIB_PATH}")
        set(CACHE_HIT TRUE)
        st_l_info_fmt(IFunctions3 ${TARGET} ${HASH})

    else ()
        set(CACHE_HIT FALSE)
        st_l_info_fmt(IFunctions4 ${TARGET})

    endif ()

    if (CACHE_HIT)
        if (DEFINED ARGS_LIBRARY_TYPE)
            add_library(${TARGET} ${ARGS_LIBRARY_TYPE} IMPORTED)
        else ()
            add_library(${TARGET} STATIC IMPORTED)
        endif ()

        set_target_properties(${TARGET} PROPERTIES
                IMPORTED_LOCATION "${LIB_PATH}"
                INTERFACE_INCLUDE_DIRECTORIES "${INCLUDE_DIR}"
                STATIONEERS_CACHE_PATH "${LIB_PATH}"
        )

        if (ARGS_PUBLIC_LINK)
            set_target_properties(${TARGET} PROPERTIES INTERFACE_LINK_LIBRARIES "${ARGS_PUBLIC_LINK}")
        endif ()

        if (ARGS_COMPILE_DEFINITIONS)
            set_target_properties(${TARGET} PROPERTIES INTERFACE_COMPILE_DEFINITIONS "${ARGS_COMPILE_DEFINITIONS}")
        endif ()

        # 缓存命中时，将未识别的参数（依赖库）也设为接口链接库
        if (ARGS_UNPARSED_ARGUMENTS)
            get_target_property(_existing_link_libs ${TARGET} INTERFACE_LINK_LIBRARIES)
            if (_existing_link_libs)
                set(_interface_libs ${_existing_link_libs} ${ARGS_UNPARSED_ARGUMENTS})
            else ()
                set(_interface_libs ${ARGS_UNPARSED_ARGUMENTS})
            endif ()
            set_target_properties(${TARGET} PROPERTIES INTERFACE_LINK_LIBRARIES "${_interface_libs}")
        endif ()

    else ()
        st_library(${TARGET} "${SOURCES}" "${INCLUDE_DIR}" ${ARGN})

        st_localize_fmt(DFunctions5 _COMMENT ${TARGET} ${LIB_NAME})
        add_custom_command(TARGET ${TARGET} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${TARGET}> "${LIB_PATH}"
                COMMENT "${_COMMENT}"
                VERBATIM
        )

    endif ()

    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${FILES}")

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
