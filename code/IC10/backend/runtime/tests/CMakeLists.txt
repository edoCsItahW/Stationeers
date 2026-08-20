include(CommonSettings)
include(Functions)


project(IC10_Runtime_Test VERSION 1.0)


st_source(IC10_RUNTIME_TEST_SRC ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(IC10_Runtime_Tests ${IC10_RUNTIME_TEST_SRC})

target_link_libraries(IC10_Runtime_Tests PRIVATE Common::Core IC10::Compiler::Core IC10::Runtime::Core GTest::gtest_main)

st_apply_compile_options(IC10_Runtime_Tests)

add_test(NAME IC10_Runtime_Tests COMMAND IC10_Runtime_Tests)

set_tests_properties(IC10_Runtime_Tests PROPERTIES
        TIMEOUT 120
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
