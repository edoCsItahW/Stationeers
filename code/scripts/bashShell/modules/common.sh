#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/debug.sh"
source "$(dirname "${BASH_SOURCE[0]}")/locale.sh"

# Detect OS key for build-info.json field lookup (windows / linux)
detect_os() {
    if [[ -n "$WINDIR" || "$OS" == "Windows_NT" ]]; then
        echo "windows"
    else
        echo "linux"
    fi
}

find_file_exists() {
    local path="$1" name="$2"
    find "$path" -type f -name "$name" 2>/dev/null | grep -q .
}

invoke_cmake_configure() {
    local target="$1" build_dir="$2" source_dir="$3"
    shift 3
    local extra_args=("$@")
    write_st_phase "$(get_text "Build.Configure.Head")"
    if [[ -d "$build_dir" ]] && find_file_exists "$build_dir" "${target}.vcxproj"; then
        write_st_info "$(get_text "Build.Configure.Skip")"
        return 0
    fi
    if [[ -d "$build_dir" ]] && [[ -f "$build_dir/CMakeCache.txt" ]]; then
        rm -f "$build_dir/CMakeCache.txt"
        write_st_info "$(get_text "Build.Configure.RemoveCache")"
    fi
    cmake -B "$build_dir" -S "$source_dir" "${extra_args[@]}" || {
        write_st_error "$(get_text "Build.Configure.Error")"
        exit 1
    }
}

invoke_cmake_build() {
    local build_dir="$1" target="$2" config="$3"
    write_st_phase "$(get_text "Build.Build.Head")"
    write_st_info "$(get_text "Build.Build.Target" "$target")"
    write_st_info "$(get_text "Build.Build.Config" "$config")"

    # 检查目标是否为 IMPORTED（缓存命中，不生成 vcxproj）
    # 仅对 Visual Studio 生成器有意义：VS 生成器会把 cmake --build 翻译成
    # msbuild <target>.vcxproj，若目标为 IMPORTED 则 .vcxproj 不存在，msbuild 报 MSB1009。
    # Ninja/Makefile 等单配置生成器对 IMPORTED 目标会直接成功，无需此检测。
    local generator=""
    if [[ -f "$build_dir/CMakeCache.txt" ]]; then
        generator=$(grep -E "^CMAKE_GENERATOR:INTERNAL=" "$build_dir/CMakeCache.txt" | cut -d= -f2-)
    fi
    if [[ "$generator" == "Visual Studio"* ]]; then
        if ! find_file_exists "$build_dir" "${target}.vcxproj"; then
            write_st_info "$(get_text "Build.Build.Cache" "$target")"
            return 0
        fi
    fi

    cmake --build "$build_dir" --target "$target" --config "$config" || {
        write_st_error "$(get_text "Build.Build.Error" "$?")"
        exit 1
    }
}

copy_artifact() {
    local src="$1" dst="$2"
    [[ -f "$src" ]] || { write_st_error "$(get_text "Build.Copy.SourceNotFound" "$src")"; exit 1; }

    if [[ "$dst" == */ ]] || [[ -d "$dst" ]]; then
        mkdir -p "$dst"
    elif [[ ! -e "$dst" ]]; then
        # Source is a file and destination doesn't exist yet — treat as
        # directory so the file is copied INTO it (e.g. .../native/).
        mkdir -p "$dst"
    else
        mkdir -p "$(dirname "$dst")"
    fi

    cp "$src" "$dst" || {
        write_st_error "$(get_text "Build.Copy.Error" "$?")"
        exit 1
    }
    write_st_info "$(get_text "Build.Copy.Success" "$src" "$dst")"
}

# Resolve artifact path across platforms.
# Multi-config generators (Visual Studio) create Release/ subdirectories;
# single-config generators (Ninja/Makefile on Linux) do not.
# This function tries the path as-is, then without /Release/.
resolve_artifact_path() {
    local artifact="$1"
    if [[ -f "$artifact" ]]; then
        echo "$artifact"
        return 0
    fi
    local stripped="${artifact/\/Release\///}"
    if [[ -f "$stripped" ]]; then
        echo "$stripped"
        return 0
    fi
    echo "$artifact"
    return 1
}
