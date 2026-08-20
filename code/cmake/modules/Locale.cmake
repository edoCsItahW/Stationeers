include(${CMAKE_CURRENT_LIST_DIR}/Debug.cmake)


set(STATIONEERS_DEFAULT_LANG "en-us")
if (NOT STATIONEERS_LANG)
    set(STATIONEERS_LANG "${STATIONEERS_DEFAULT_LANG}" CACHE STRING "Language for build message (e.g., en-us, zh-hans)")
endif ()

set(_ST_LOCALE_DIR "${CMAKE_CURRENT_LIST_DIR}/locales")

macro(_st_load_locale)

    if (NOT DEFINED _ST_LOADED_LANG OR NOT "${_ST_LOADED_LANG}" STREQUAL "${STATIONEERS_LANG}")
        set(CURRENT_LOCALE_FILE "${_ST_LOCALE_DIR}/${STATIONEERS_LANG}.cmake")

        if (EXISTS ${CURRENT_LOCALE_FILE})
            include(${CURRENT_LOCALE_FILE})

        else ()
            include(${_ST_LOCALE_DIR}/${STATIONEERS_DEFAULT_LANG}.cmake)

        endif ()

        set(_ST_LOADED_LANG "${STATIONEERS_LANG}")

    endif ()

endmacro()


macro(st_localize MSG_ID OUT_VAR)
    _st_load_locale()

    set(ST_LOC_VAR "ST_${MSG_ID}")

    if (DEFINED ${ST_LOC_VAR})
        set(${OUT_VAR} "${${ST_LOC_VAR}}")
    else ()
        set(${OUT_VAR} "[${STATIONEERS_LANG}_${MSG_ID}]")
    endif ()

    unset(ST_LOC_VAR)
endmacro()


function(st_localize_fmt MSG_ID OUT_VAR)

    st_localize(${MSG_ID} _CONTENT)

    set(_AGR_IDX 0)

    foreach (_ARG ${ARGN})
        string(REPLACE "{${_AGR_IDX}}" "${_ARG}" _CONTENT "${_CONTENT}")
        math(EXPR _AGR_IDX "${_AGR_IDX} + 1")
    endforeach ()

    set(${OUT_VAR} "${_CONTENT}" PARENT_SCOPE)

endfunction()


macro(l_message MSG_ID)
    st_localize(${MSG_ID} _CONTENT)

    message(${_CONTENT})

    unset(_CONTENT)
endmacro()


macro(l_message_fmt MSG_ID)
    st_localize_fmt(${MSG_ID} _CONTENT ${ARGN})

    message(${_CONTENT})

    unset(_CONTENT)
endmacro()


function(st_l_debug MSG_ID)

    st_localize(${MSG_ID} _CONTENT)

    st_debug(${_CONTENT})

endfunction()

function(st_l_debug_fmt MSG_ID)

    st_localize_fmt(${MSG_ID} _CONTENT ${ARGN})

    st_debug(${_CONTENT})

endfunction()


function(st_l_info MSG_ID)

    st_localize(${MSG_ID} _CONTENT)

    st_info(${_CONTENT})

endfunction()

function(st_l_info_fmt MSG_ID)

    st_localize_fmt(${MSG_ID} _CONTENT ${ARGN})

    st_info(${_CONTENT})

endfunction()


function(st_l_warning MSG_ID)

    st_localize(${MSG_ID} _CONTENT)

    st_warning(${_CONTENT})

endfunction()

function(st_l_warning_fmt MSG_ID)

    st_localize_fmt(${MSG_ID} _CONTENT ${ARGN})

    st_warning(${_CONTENT})

endfunction()


function(st_l_error MSG_ID)

    st_localize(${MSG_ID} _CONTENT)

    st_error(${_CONTENT})

endfunction()


function(st_l_error_fmt MSG_ID)

    st_localize_fmt(${MSG_ID} _CONTENT ${ARGN})

    st_error(${_CONTENT})

endfunction()
