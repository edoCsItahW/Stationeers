project(IC10_Runtime VERSION 1.0)

add_subdirectory(core)

if (ANY_EXPORTS)
    add_subdirectory(exports)
endif ()


if (ANY_TESTS)
    add_subdirectory(tests)
endif ()
