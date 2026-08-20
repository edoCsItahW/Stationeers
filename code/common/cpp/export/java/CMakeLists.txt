project(Common_Java VERSION 1.0)


st_source(COMMON_JAVA_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)
st_auto_library(common_java "${COMMON_JAVA_SRC}" ${CMAKE_CURRENT_SOURCE_DIR}/include PRIVATE_LINK Common::Core JNI::JNI fbjni::fbjni)

set_target_properties(common_java PROPERTIES PREFIX "")

st_apply_compile_options(common_java)

st_alias(Common::Java common_java)
