function(st_apply_compile_options TARGET)

    get_target_property(_imported ${TARGET} IMPORTED)
    if (_imported)
        set(_visibility INTERFACE)
    else ()
        set(_visibility PRIVATE)
    endif ()

    target_compile_options(${TARGET} ${_visibility}
            # 通用优化
            $<IF:$<CONFIG:Debug>,
            $<IF:$<CXX_COMPILER_ID:MSVC>, /Od, -O0>,  # Debug模式下禁用优化
            $<IF:$<CXX_COMPILER_ID:MSVC>, /O2, -O2>
            >

            # 编译器警告
            $<IF:$<CXX_COMPILER_ID:MSVC>,
            /diagnostics:caret  # 让错误和警告信息在控制台输出时，精准显示一个插入符号（^）指向出错的具体列位置，极大提升排查错误的效率。
            /Zc:__cplusplus     # MSVC 默认不报告符合标准的 __cplusplus 宏值（默认为 199711L）。加上此选项后，编译器会根据 /std:c++latest 等标准返回正确的值（如 202002L），用于让依赖此宏的第三方库正确识别 C++ 版本。
            /wd4819             # 禁用警告 C4819。该警告通常指源文件包含无法在当前代码页（如中文 GBK）中表示的字符，保留此选项用于兼容旧代码。
            /bigobj             # 允许生成的 .obj 目标文件包含超过 65535 个节（section）。必须开启，否则会报链接错误。
            /utf-8              # 将编译器的源字符集和执行字符集都设置为 UTF-8，确保源码中的中文等 Unicode 字符被正确处理，避免乱码或编译错误。
            /MP                 # 多处理器编译。编译时启用多进程并行处理源文件，显著加快大型项目的构建速度。
            ,
            -Wall -Wextra  # 启用绝大多数常见警告（Wall）和额外的、较严格的警告（Wextra）。
            >
    )

endfunction()


macro(enable_compiler_cache)

    find_program(SCCACHE_PROGRAM sccache)

    if (SCCACHE_PROGRAM)
        set(CMAKE_CXX_COMPILER_LAUNCHER ${SCCACHE_PROGRAM} CACHE STRING "" FORCE)
        set(CMAKE_C_COMPILER_LAUNCHER ${SCCACHE_PROGRAM} CACHE STRING "" FORCE)

        st_l_info_fmt(ICommonSettings0 "sccache" "${CMAKE_CXX_COMPILER_LAUNCHER}")

    else ()
        find_program(CCACHE_PROGRAM ccache)

        if (CCACHE_PROGRAM)
            set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM} CACHE STRING "" FORCE)
            set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM} CACHE STRING "" FORCE)

            st_l_info_fmt(ICommonSettings0 "ccache" "${CMAKE_CXX_COMPILER_LAUNCHER}")

        endif ()

    endif ()

endmacro()


macro(limit_linked_memory)
    if (NOT DEFINED CMAKE_JOB_POOL_LINK)
        set(CMAKE_JOB_POOL_LINK "link_job" CACHE STRING "" FORCE)
    endif ()
    if (NOT DEFINED CMAKE_JOB_POOLS)
        set(CMAKE_JOB_POOLS "link_job=3" CACHE STRING "" FORCE)
    endif ()
endmacro()


function(auto_find_linker)
    find_program(MOLD_EXE mold)

    if (MOLD_EXE AND NOT DEFINED CMAKE_LINKER_TYPE)
        # 仅应用于可执行文件和共享库，避免影响 MODULE 库（pybind11/N-API）
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=mold" CACHE STRING "" FORCE)
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=mold" CACHE STRING "" FORCE)
        st_l_info_fmt(ICommonSettings0 "mold" "${MOLD_EXE}")

    else ()
        find_program(LLD_EXE lld)

        if (LLD_EXE AND NOT DEFINED CMAKE_LINKER_TYPE)
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld" CACHE STRING "" FORCE)
            set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=lld" CACHE STRING "" FORCE)
            st_l_info_fmt(ICommonSettings0 "lld" "${LLD_EXE}")

        endif ()

    endif ()

endfunction()
