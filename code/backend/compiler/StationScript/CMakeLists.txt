add_executable(high_level_compiler)

target_include_directories(high_level_compiler
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${COMMON_INCLUDE_DIR}
)

file(GLOB_RECURSE HLC_SOURCES "src/*.cpp")
target_sources(high_level_compiler PRIVATE ${HLC_SOURCES})

target_link_libraries(high_level_compiler PRIVATE common)

# 编译选项
target_compile_features(high_level_compiler PRIVATE cxx_std_23)
