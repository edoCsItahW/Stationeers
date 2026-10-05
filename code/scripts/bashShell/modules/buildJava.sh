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
    local artifact_dir=$(jq -r '.ArtifactPath' <<<"$config_json")
    local os=$(detect_os)
    local artifact_name=$(jq -r ".ArtifactName.${os}" <<<"$config_json")
    local artifact="${artifact_dir}/${artifact_name}"
    local publish_dir=$(jq -r '.PublishDir' <<<"$config_json")
    local stdlib=$(jq -r '.StdLibPath' <<<"$config_json")
    local stdlib_dir=$(jq -r '.StdLibDir' <<<"$config_json")
    local test_dir=$(jq -r '.TestDir' <<<"$config_json")
    local test_script=$(jq -r ".TestScript.${os}" <<<"$config_json")

    invoke_cmake_configure "$target" "$build_dir" "$source_dir" "${extra_args[@]}"
    invoke_cmake_build "$build_dir" "$target" "$config_type"

    write_st_phase "$(get_text "Java.Copy")"
    local resolved_artifact
    resolved_artifact=$(resolve_artifact_path "$artifact")
    copy_artifact "$resolved_artifact" "$publish_dir"

    write_st_phase "$(get_text "Build.StdLib.Head")"

    # 标准库只有声明了 StdLibDir 的目标才有；这里放进 jar 的 resources 根（.dll 在 native/ 下）；
    # jq 读不到键时会给出字面量 null，所以两种空值都要挡掉
    if [[ -z "$stdlib_dir" || "$stdlib_dir" == "null" ]]; then
        write_st_info "$(get_text "Build.StdLib.Skipped" "$target")"
    else
        sync_stdlib "$stdlib" "$stdlib_dir"
    fi

    write_st_phase "$(get_text "Java.Test")"
    (cd "$test_dir" && {
        gradlew_path="../../publish/java/${test_script}"
        if [[ "$os" == "linux" ]]; then
            chmod +x "$gradlew_path" 2>/dev/null || true
        fi
        libs_dir="$(realpath ../../publish/java/src/main/resources/native 2>/dev/null || true)"
        if [[ -n "$libs_dir" ]]; then
            export LD_LIBRARY_PATH="$libs_dir:${LD_LIBRARY_PATH:-}"
        fi
        ./"$gradlew_path" test --no-daemon --stacktrace
    }) || {
        write_st_error "$(get_text "Java.Error" "$?")"
        exit 1
    }
}