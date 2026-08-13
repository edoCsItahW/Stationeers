include(${CMAKE_CURRENT_LIST_DIR}/modules/Locale.cmake)


find_package(Python REQUIRED COMPONENTS Interpreter Development)

st_localize(DPybind110 _PYBIND11_AUTO_INSTALL_DOC)
option(PYBIND11_AUTO_INSTALL ${_PYBIND11_AUTO_INSTALL_DOC} ON)

if (NOT pybind11_DIR AND NOT DEFINED ENV{PYBIND11_DIR})

    st_l_info(IPybind111)

    execute_process(
            COMMAND ${Python_EXECUTABLE} -c "import pybind11; print(pybind11.get_cmake_dir())"
            OUTPUT_VARIABLE _pybind11_cmake_dir
            RESULT_VARIABLE _pybind11_result
            ERROR_VARIABLE _pybind11_error
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if (_pybind11_result EQUAL 0 AND _pybind11_cmake_dir)
        file(TO_CMAKE_PATH "${_pybind11_cmake_dir}" _pybind11_cmake_dir)

        st_localize(DPybind112 _PYBIND11_MSG)

        set(pybind11_DIR "${_pybind11_cmake_dir}" CACHE PATH ${_PYBIND11_MSG} FORCE)

        st_l_info_fmt(IPybind113 ${pybind11_DIR})

    else ()

        st_l_info_fmt(IPybind114 ${_pybind11_result} ${_pybind11_error})

    endif ()

endif ()

macro(_find_pybind11_config)
    if (pybind11_DIR)
        find_package(pybind11 CONFIG REQUIRED)

    else ()
        find_package(pybind11 CONFIG QUIET)


    endif ()

endmacro()

_find_pybind11_config()

if (NOT pybind11_FOUND AND PYBIND11_AUTO_INSTALL)
    st_l_info(IPybind115)

    set(_pip_cmd ${Python_EXECUTABLE} -m pip install pybind11)

    if (PYBIND11_PIP_OPTIONS)
        list(APPEND _pip_cmd ${PYBIND11_PIP_OPTIONS})
    endif ()

    execute_process(
            COMMAND ${_pip_cmd}
            RESULT_VARIABLE _pybind11_result
            ERROR_VARIABLE _pybind11_error
            OUTPUT_QUIET
    )

    if (_pybind11_result EQUAL 0)
        st_l_info(IPybind116)

        unset(pybind11_DIR CACHE)
        unset(pybind11_FOUND)

        execute_process(
                COMMAND ${Python_EXECUTABLE} -c "import pybind11; print(pybind11.get_cmake_dir())"
                OUTPUT_VARIABLE _pybind11_cmake_dir
                RESULT_VARIABLE _pybind11_result
                ERROR_VARIABLE _pybind11_error
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        if (_pybind11_result EQUAL 0 AND _pybind11_cmake_dir)
            file(TO_CMAKE_PATH "${_pybind11_cmake_dir}" _pybind11_cmake_dir)

            set(pybind11_DIR "${_pybind11_cmake_dir}" CACHE PATH ${_PYBIND11_MSG} FORCE)

            st_l_info_fmt(IPybind117 ${pybind11_DIR})

        else ()

            st_l_info(IPybind118)

        endif ()

        _find_pybind11_config()

    else ()

        st_l_info_fmt(IPybind119 ${_pybind11_error})

    endif ()

endif ()


include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(pybind11
        REQUIRED_VARS pybind11_DIR Python_EXECUTABLE
        VERSION_VAR pybind11_VERSION
        HANDLE_COMPONENTS
)


if (NOT pybind11_FOUND)
    st_l_info(IPybind1110)

    return()

endif ()


if (NOT TARGET pybind11::pybind11)
    st_l_info(IPybind1111)

endif ()


get_target_property(_pybind11_include_dirs pybind11::pybind11 INTERFACE_INCLUDE_DIRECTORIES)
if (_pybind11_include_dirs)
    set(pybind11_INCLUDE_DIRS "${_pybind11_include_dirs}")

else ()
    get_filename_component(_pybind11_prefix "${pybind11_DIR}/../../.." ABSOLUTE)

    st_localize(DPybind1112 _PYBIND11_DIR_DOC)
    set(pybind11_INCLUDE_DIRS "${_pybind11_prefix}/include" CACHE PATH ${_PYBIND11_DIR_DOC})

endif ()

set(pybind11_LIBRARIES "")


st_l_info_fmt(IPybind1113 ${Python_EXECUTABLE})
st_l_info_fmt(IPybind1114 ${pybind11_DIR})
st_l_info_fmt(IPybind1115 ${pybind11_INCLUDE_DIRS})
st_l_info(IPybind1116)
