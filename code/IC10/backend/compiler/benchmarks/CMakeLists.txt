project(IC10_Compiler_Benchmark VERSION 1.0)

st_source(IC10_COMPILER_BENCH_SRC ${CMAKE_CURRENT_SOURCE_DIR})

st_auto_executable(IC10_Compiler_Benchmark "${IC10_COMPILER_BENCH_SRC}" "" PRIVATE_LINK Common::Core IC10::Compiler::Core)

st_apply_compile_options(IC10_Compiler_Benchmark)
