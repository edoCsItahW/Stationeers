#!/usr/bin/env bash
# 日志和阶段输出函数

write_st_message() {
    local level="$1"
    local message="$2"
    case "$level" in
        DEBUG)   echo "[STATIONEERS] - Debug: $message" ;;
        INFO)    echo -e "\033[32m[STATIONEERS] - Info: $message\033[0m" ;;
        WARNING) echo -e "\033[33m[STATIONEERS] - Warning: $message\033[0m" ;;
        ERROR)   echo -e "\033[31m[STATIONEERS] - Error: $message\033[0m" >&2 ;;
    esac
}

write_st_debug()   { write_st_message "DEBUG" "$1"; }
write_st_info()    { write_st_message "INFO" "$1"; }
write_st_warning() { write_st_message "WARNING" "$1"; }
write_st_error()   { write_st_message "ERROR" "$1"; }

write_st_phase() {
    local msg="$1"
    local len=${#msg}
    local delim=$(printf '/%*s' $((len+42)) '' | tr ' ' '/')
    echo "$delim"
    printf '%*s %s %*s\n' 20 '' "$msg" 20 ''
    echo "$delim"
}