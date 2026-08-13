#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/debug.sh"
source "$(dirname "${BASH_SOURCE[0]}")/locale.sh"

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
    mkdir -p "$(dirname "$dst")"
    cp "$src" "$dst" || {
        write_st_error "$(get_text "Build.Copy.Error" "$?")"
        exit 1
    }
    write_st_info "$(get_text "Build.Copy.Success" "$src" "$dst")"
}

# Resolve artifact path across platforms.
# build-info.json stores Windows paths (with Release/ subdirectory and .dll/.pyd extensions).
# On Linux (single-config generators), the path differs (no Release/, .so extension with lib prefix).
resolve_artifact_path() {
    local artifact="$1"
    # 1. Try the path as-is (works on Windows or if already correct)
    if [[ -f "$artifact" ]]; then
        echo "$artifact"
        return 0
    fi
    # 2. Try without Release/ subdirectory (Linux single-config generator)
    local stripped="${artifact/\/Release\///}"
    if [[ -f "$stripped" ]]; then
        echo "$stripped"
        return 0
    fi
    # 3. Try finding by filename (for Linux where path structure may differ)
    local filename
    filename=$(basename "$artifact")
    local found
    found=$(find build -name "$filename" -type f 2>/dev/null | head -1)
    if [[ -n "$found" ]]; then
        echo "$found"
        return 0
    fi
    # 4. For Java: .dll -> lib*.so on Linux
    if [[ "$filename" == *.dll ]]; then
        local base="${filename%.dll}"
        found=$(find build -name "lib${base}.so" -type f 2>/dev/null | head -1)
        if [[ -n "$found" ]]; then
            echo "$found"
            return 0
        fi
    fi
    # 5. For Windows executables: .exe -> no extension on Linux
    if [[ "$filename" == *.exe ]]; then
        local base="${filename%.exe}"
        found=$(find build -name "$base" -type f 2>/dev/null | head -1)
        if [[ -n "$found" ]]; then
            echo "$found"
            return 0
        fi
    fi
    # Not found, return original path (copy_artifact will report the error)
    echo "$artifact"
    return 1
}