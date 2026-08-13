include(CommonSettings)
include(Functions)

project(IC10_Compiler_Core)

set(IC10_COMPILER_CORE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

st_source(IC10_COMPILER_CORE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB_RECURSE IC10_COMPILER_CORE_PCH CONFIGURE_DEPENDS "${IC10_COMPILER_CORE_INCLUDE_DIR}/ic10_compiler/pch/*.hpp")
st_auto_library(ic10_compiler_core "${IC10_COMPILER_CORE_SRC}" "${IC10_COMPILER_CORE_INCLUDE_DIR}" PUBLIC_LINK Common::Core PCH_MODE PRIVATE PCH "${IC10_COMPILER_CORE_PCH}")

st_apply_compile_options(ic10_compiler_core)

st_alias(IC10::Compiler::Core ic10_compiler_core)


add_executable(IC10_Compiler_CLI main.cpp)
target_link_libraries(IC10_Compiler_CLI PRIVATE Common::Core IC10::Compiler::Core)

set_target_properties(IC10_Compiler_CLI PROPERTIES OUTPUT_NAME "ic10c" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

st_apply_compile_options(IC10_Compiler_CLI)
