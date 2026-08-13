project(Common_Python VERSION 1.0)


st_source(COMMON_PYTHON_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

st_auto_library(common_python "${COMMON_PYTHON_SRC}" "${CMAKE_CURRENT_SOURCE_DIR}/include" Common::Core pybind11::pybind11)

st_apply_compile_options(common_python)

st_alias(Common::Python common_python)
