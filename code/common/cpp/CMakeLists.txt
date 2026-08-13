## 使用 lld
#if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
#    target_link_options(common INTERFACE -fuse-ld=lld)
#endif()
project(Common VERSION 1.0)

add_subdirectory(core)

if (ANY_EXPORTS)
    add_subdirectory(export)
endif ()

if (ANY_TESTS)
    add_subdirectory(tests)
endif ()
