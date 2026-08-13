#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"

build_java() {
    local config_json="$1"
    shift
    local extra_args=("$@")
    local build_dir=$(jq -r '.BuildDir' <<<"$config_json")
    local source_dir=$(jq -r '.SourceDir' <<<"$config_json")
    local target=$(jq -r '.Target' <<<"$config_json")
    local config_type=$(jq -r '.Config' <<<"$config_json")
    local artifact=$(jq -r '.ArtifactPath' <<<"$config_json")
    local publish_dir=$(jq -r '.PublishDir' <<<"$config_json")
    local test_dir=$(jq -r '.TestDir' <<<"$config_json")

    invoke_cmake_configure "$target" "$build_dir" "$source_dir" "${extra_args[@]}"
    invoke_cmake_build "$build_dir" "$target" "$config_type"

    write_st_phase "$(get_text "Java.Copy")"
    local resolved_artifact
    resolved_artifact=$(resolve_artifact_path "$artifact")
    copy_artifact "$resolved_artifact" "$publish_dir"

    write_st_phase "$(get_text "Java.Test")"
    (cd "$test_dir" && {
        # Locate the gradle wrapper shipped with the publish/java package.
        # The test directory does not contain gradlew; it lives in publish/java.
        gradlew_cmd="gradle"
        gradlew_path="../../publish/java/gradlew"
        gradlew_bat_path="../../publish/java/gradlew.bat"
        if [[ -f "$gradlew_bat_path" ]]; then
            gradlew_cmd="$gradlew_bat_path"
        elif [[ -f "$gradlew_path" ]]; then
            chmod +x "$gradlew_path" 2>/dev/null || true
            gradlew_cmd="./$gradlew_path"
        fi
        # On Linux, ensure the native library is discoverable via LD_LIBRARY_PATH.
        libs_dir="$(realpath ../../publish/java/src/main/resources/native 2>/dev/null || true)"
        if [[ -n "$libs_dir" ]]; then
            export LD_LIBRARY_PATH="$libs_dir:${LD_LIBRARY_PATH:-}"
        fi
        $gradlew_cmd test --no-daemon --stacktrace
    }) || {
        write_st_error "$(get_text "Java.Error" "$?")"
        exit 1
    }
}