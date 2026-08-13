project(IC10_Compiler_Test VERSION 1.0)

st_source(IC10_COMPILER_TEST_SRC ${CMAKE_CURRENT_SOURCE_DIR}/cpp)
add_executable(IC10_Compiler_Tests ${IC10_COMPILER_TEST_SRC})

target_link_libraries(IC10_Compiler_Tests PRIVATE Common::Core IC10::Compiler::Core GTest::gtest_main)

st_apply_compile_options(IC10_Compiler_Tests)

add_test(NAME IC10_Compiler_Tests COMMAND IC10_Compiler_Tests)

set_tests_properties(IC10_Compiler_Tests PROPERTIES
        TIMEOUT 120
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
