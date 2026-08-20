function(st_message LEVEL MESSAGE)
    if (NOT MESSAGE)
        return()
    endif ()

    if (LEVEL STREQUAL "INFO")
        message(STATUS "[STATIONEERS] - ${LEVEL}: ${MESSAGE}")
    elseif (LEVEL STREQUAL "WARNING")
        message(WARNING "[STATIONEERS] - ${LEVEL}: ${MESSAGE}")
    elseif (LEVEL STREQUAL "ERROR")
        message(FATAL_ERROR "[STATIONEERS] - ${LEVEL}: ${MESSAGE}")
    elseif (LEVEL STREQUAL "DEBUG")
        message(DEBUG "[STATIONEERS] - ${LEVEL}: ${MESSAGE}")
    endif ()

endfunction()


function(st_debug MESSAGE)

    st_message(DEBUG ${MESSAGE})

endfunction()


function(st_info MESSAGE)

    st_message(INFO ${MESSAGE})

endfunction()

function(st_warning MESSAGE)

    st_message(WARNING ${MESSAGE})

endfunction()

function(st_error MESSAGE)

    st_message(ERROR ${MESSAGE})

endfunction()
