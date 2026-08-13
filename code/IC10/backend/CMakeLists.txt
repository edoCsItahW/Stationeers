if (ANY_RUNTIME)
    add_subdirectory(runtime)

    set(ANY_COMPILER ON)
endif ()

if (ANY_COMPILER)
    add_subdirectory(compiler)

endif ()
