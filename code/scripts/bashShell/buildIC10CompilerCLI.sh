#!/usr/bin/env bash
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/modules/debug.sh"
source "$SCRIPT_DIR/modules/locale.sh"
source "$SCRIPT_DIR/modules/common.sh"

initialize_locale "${STATIONEERS_LANG:-"zh-hans"}"

CONFIG_FILE="$SCRIPT_DIR/../build-info.json"
[[ -f "$CONFIG_FILE" ]] || { write_st_error "build-info.json not found!"; exit 1; }

PROJECT=$(cat "$CONFIG_FILE")

ROOT_DIR_REL=$(jq -r ".rootDir" <<<"$PROJECT")
[[ "$ROOT_DIR_REL" != "null" ]] || { write_st_error "rootDir not found in build-info.json"; exit 1; }
ROOT_DIR="$(cd "$SCRIPT_DIR/$ROOT_DIR_REL" && pwd)"

TARGET_KEY="IC10_Compiler_CLI"
CONFIG_JSON=$(jq -r ".\"$TARGET_KEY\"" <<<"$PROJECT")
[[ "$CONFIG_JSON" != "null" ]] || { write_st_error "Target '$TARGET_KEY' not found"; exit 1; }

EXTRA_ARGS=(
    "-DSTATIONEERS_LANG=zh-hans"
    "-DBUILD_IC10_COMPILER_CORE=ON"
)

BUILD_DIR=$(jq -r '.BuildDir' <<<"$CONFIG_JSON")
SOURCE_DIR=$(jq -r '.SourceDir' <<<"$CONFIG_JSON")
TARGET=$(jq -r '.Target' <<<"$CONFIG_JSON")
CONFIG_TYPE=$(jq -r '.Config' <<<"$CONFIG_JSON")
TEST_DIR=$(jq -r '.TestDir' <<<"$CONFIG_JSON")
TEST_EXE_DIR=$(jq -r '.TestExeDir' <<<"$CONFIG_JSON")
OS=$(detect_os)
TEST_EXE_NAME=$(jq -r ".TestExeName.${OS}" <<<"$CONFIG_JSON")
TEST_EXE="${TEST_EXE_DIR}/${TEST_EXE_NAME}"

cd "$ROOT_DIR"

invoke_cmake_configure "$TARGET" "$BUILD_DIR" "$SOURCE_DIR" "${EXTRA_ARGS[@]}"
invoke_cmake_build "$BUILD_DIR" "$TARGET" "$CONFIG_TYPE"

write_st_phase "$(get_text "Core.Test")"

ACTUAL_TEST_EXE=$(resolve_artifact_path "$TEST_EXE")

if [[ ! -f "$ACTUAL_TEST_EXE" ]]; then
    write_st_error "$(get_text "Build.Copy.SourceNotFound" "$ACTUAL_TEST_EXE")"
    exit 1
fi

cd "$TEST_DIR"
./"$ACTUAL_TEST_EXE" --version || {
    write_st_error "$(get_text "Core.Error" "$?")"
    exit 1
}
