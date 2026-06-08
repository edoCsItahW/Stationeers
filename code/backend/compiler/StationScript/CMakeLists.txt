add_library(station_script_lib STATIC)

file(GLOB_RECURSE LIB_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.cpp"
)

# 假设 StationScript 也有 main.cpp（类似主程序入口）
list(REMOVE_ITEM LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

target_sources(station_script_lib PRIVATE ${LIB_SOURCES})

target_include_directories(station_script_lib
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${COMMON_INCLUDE_DIR}
)

target_link_libraries(station_script_lib PUBLIC common)

#add_executable(Station_Script_Compiler main.cpp)
#target_link_libraries(Station_Script_Compiler PRIVATE station_script_lib common)