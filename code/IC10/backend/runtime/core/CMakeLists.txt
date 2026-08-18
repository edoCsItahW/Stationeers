include(CommonSettings)
include(Functions)

project(IC10_Runtime_Core)

st_source(IC10_RUNTIME_CORE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)
st_auto_library(ic10_runtime_core "${IC10_RUNTIME_CORE_SRC}" ${CMAKE_CURRENT_SOURCE_DIR}/include PRIVATE_LINK Common::Core IC10::Compiler::Core)

st_apply_compile_options(ic10_runtime_core)

st_alias(IC10::Runtime::Core ic10_runtime_core)
