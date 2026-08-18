#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"

build_python() {
    local config_json="$1"
    shift
    local extra_args=("$@")
    local build_dir=$(jq -r '.BuildDir' <<<"$config_json")
    local source_dir=$(jq -r '.SourceDir' <<<"$config_json")
    local target=$(jq -r '.Target' <<<"$config_json")
    local config_type=$(jq -r '.Config' <<<"$config_json")
    local artifact_dir=$(jq -r '.ArtifactPath' <<<"$config_json")
    local os=$(detect_os)
    local artifact_name=$(jq -r ".ArtifactName.${os}" <<<"$config_json")
    local artifact="${artifact_dir}/${artifact_name}"
    local publish_dir=$(jq -r '.PublishDir' <<<"$config_json")
    local test_dir=$(jq -r '.TestDir' <<<"$config_json")

    invoke_cmake_configure "$target" "$build_dir" "$source_dir" "${extra_args[@]}"
    invoke_cmake_build "$build_dir" "$target" "$config_type"

    write_st_phase "$(get_text "Python.Copy")"
    local resolved_artifact
    resolved_artifact=$(resolve_artifact_path "$artifact")
    copy_artifact "$resolved_artifact" "$publish_dir"

    write_st_phase "$(get_text "Python.Test")"
    (cd "$test_dir" && pip install --no-input pytest && python -m pytest) || {
        write_st_error "$(get_text "Python.Error" "$?")"
        exit 1
    }
}