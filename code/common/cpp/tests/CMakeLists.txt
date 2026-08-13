include(Functions)


project(Common_Test VERSION 1.0)

st_source(COMMON_TEST_SRC ${CMAKE_CURRENT_SOURCE_DIR})
st_library(common_test "${COMMON_TEST_SRC}" "" Common::Core GTest::gtest)

st_alias(Common::Test common_test)
