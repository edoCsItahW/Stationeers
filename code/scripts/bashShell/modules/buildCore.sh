#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"

build_core() {
    local config_json="$1"
    shift
    local extra_args=("$@")
    local build_dir=$(jq -r '.BuildDir' <<<"$config_json")
    local source_dir=$(jq -r '.SourceDir' <<<"$config_json")
    local target=$(jq -r '.Target' <<<"$config_json")
    local config_type=$(jq -r '.Config' <<<"$config_json")
    local test_dir=$(jq -r '.TestDir' <<<"$config_json")
    local test_exe_dir=$(jq -r '.TestExeDir' <<<"$config_json")
    local os=$(detect_os)
    local test_exe_name=$(jq -r ".TestExeName.${os}" <<<"$config_json")
    local test_exe="${test_exe_dir}/${test_exe_name}"

    invoke_cmake_configure "$target" "$build_dir" "$source_dir" "${extra_args[@]}"
    invoke_cmake_build "$build_dir" "$target" "$config_type"

    write_st_phase "$(get_text "Core.Test")"

    local actual_test_exe
    actual_test_exe=$(resolve_artifact_path "$test_exe")

    if [[ ! -f "$actual_test_exe" ]]; then
        write_st_error "$(get_text "Build.Copy.SourceNotFound" "$actual_test_exe")"
        exit 1
    fi

    (cd "$test_dir" && ./"$actual_test_exe") || {
        write_st_error "$(get_text "Core.Error" "$?")"
        exit 1
    }
}
