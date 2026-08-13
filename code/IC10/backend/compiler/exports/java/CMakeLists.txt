project(IC10_Compiler_Java VERSION 1.0)

st_source(IC10_COMPILER_JAVA_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)
st_auto_library(ic10_compiler_java "${IC10_COMPILER_JAVA_SRC}" ${CMAKE_CURRENT_SOURCE_DIR}/include JNI::JNI fbjni::fbjni IC10::Compiler::Core Common::Java LIBRARY_TYPE SHARED)

set_target_properties(ic10_compiler_java PROPERTIES OUTPUT_NAME "ic10c_java")

st_is_imported(ic10_compiler_java _imported)
if (NOT _imported)
    if(UNIX AND NOT APPLE)
        target_link_options(ic10_compiler_java PRIVATE -Wl,--allow-multiple-definition)
    endif()
endif()

st_apply_compile_options(ic10_compiler_java)

st_alias(IC10::Compiler::Java ic10_compiler_java)

