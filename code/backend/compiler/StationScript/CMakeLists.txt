add_executable(Station_Script_Compiler)

target_include_directories(Station_Script_Compiler
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${COMMON_INCLUDE_DIR}
)

file(GLOB_RECURSE HLC_SOURCES "src/**/*.cpp")
target_sources(Station_Script_Compiler PRIVATE ${HLC_SOURCES})

target_link_libraries(Station_Script_Compiler PRIVATE common)

# 编译选项
target_compile_features(Station_Script_Compiler PRIVATE cxx_std_23)
