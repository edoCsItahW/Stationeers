add_library(test_utils STATIC
        test_utils.cpp
)
target_link_libraries(test_utils PRIVATE common gtest)
target_include_directories(test_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})