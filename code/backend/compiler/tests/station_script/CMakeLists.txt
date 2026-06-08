#add_executable(station_script_tests
#        lexer_test.cpp
#        parser_test.cpp
#        # ... 其他测试文件
#)
#
#target_link_libraries(station_script_tests
#        PRIVATE
#        common
#        station_script_lib
#        gtest_main
#)
#
#target_include_directories(station_script_tests PRIVATE
#        ${CMAKE_SOURCE_DIR}/StationScript/include
#        ${CMAKE_SOURCE_DIR}/common/include
#)
#
#add_test(NAME station_script_tests COMMAND station_script_tests)