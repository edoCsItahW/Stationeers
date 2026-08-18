include(CommonSettings)
include(Functions)


project(IC10_Compiler_Python VERSION 1.0)


st_source(IC10_COMPILER_PYTHON_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)

pybind11_add_module(ic10c_python ${IC10_COMPILER_PYTHON_SRC})

target_include_directories(ic10c_python PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(ic10c_python PRIVATE Common::Core IC10::Compiler::Core Common::Python)

if (WIN32)
    set_target_properties(ic10c_python PROPERTIES SUFFIX ".pyd")
else ()
    set_target_properties(ic10c_python PROPERTIES SUFFIX ".so")
endif ()

st_apply_compile_options(ic10c_python)

st_alias(IC10::Compiler::Python ic10c_python)
