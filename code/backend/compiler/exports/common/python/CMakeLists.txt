project(Common_Python_Build VERSION 1.0)

# ============================================================================
# 依赖查找（pybind11）
# ============================================================================
include(${CMAKE_SOURCE_DIR}/cmake/pybind11.cmake)

# ============================================================================
# 主要构建目标
# ============================================================================
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_library(common_python STATIC ${SOURCE})
target_include_directories(common_python PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(common_python
        PUBLIC
        pybind11::pybind11
        common
)
