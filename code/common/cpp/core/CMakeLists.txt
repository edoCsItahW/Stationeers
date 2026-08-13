include(CommonSettings)
include(Functions)


set(COMMON_CORE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
st_source(COMMON_CORE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB_RECURSE COMMON_CORE_PCH CONFIGURE_DEPENDS "${COMMON_CORE_INCLUDE_DIR}/pch/*.hpp")
st_auto_library(common_core "${COMMON_CORE_SRC}" "${COMMON_CORE_INCLUDE_DIR}" PCH "${COMMON_CORE_PCH}")

st_apply_compile_options(common_core)

st_alias(Common::Core common_core)
