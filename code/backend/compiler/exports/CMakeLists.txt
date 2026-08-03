if(BUILD_NODE_BINDINGS)
    add_subdirectory(common/node)
endif()

if(BUILD_PYTHON_BINDINGS)
    add_subdirectory(common/python)
endif()

if(BUILD_JAVA_BINDINGS)
    add_subdirectory(common/java)
endif()

if(BUILD_IC10 AND BUILD_NODE_BINDINGS)
    add_subdirectory(IC10/node)
endif()

if(BUILD_IC10 AND BUILD_PYTHON_BINDINGS)
    add_subdirectory(IC10/python)
endif()

if(BUILD_IC10 AND BUILD_JAVA_BINDINGS)
    add_subdirectory(IC10/java)
endif()

if(BUILD_STATIONSCRIPT AND BUILD_NODE_BINDINGS)
    # add_subdirectory(StationScript/node)
endif()

if(BUILD_STATIONSCRIPT AND BUILD_PYTHON_BINDINGS)
    # add_subdirectory(StationScript/python)
endif()