#!/usr/bin/env bash
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/modules/debug.sh"
source "$SCRIPT_DIR/modules/locale.sh"
source "$SCRIPT_DIR/modules/buildJava.sh"

initialize_locale "${STATIONEERS_LANG:-"zh-hans"}"

CONFIG_FILE="$SCRIPT_DIR/../build-info.json"
[[ -f "$CONFIG_FILE" ]] || { write_st_error "build-info.json not found!"; exit 1; }

ROOT_DIR_REL=$(jq -r ".rootDir" "$CONFIG_FILE")
[[ "$ROOT_DIR_REL" != "null" ]] || { write_st_error "rootDir not found in build-info.json"; exit 1; }
ROOT_DIR="$(cd "$SCRIPT_DIR/$ROOT_DIR_REL" && pwd)"
cd "$ROOT_DIR"

TARGET_KEY="IC10_Compiler_Java"
CONFIG_JSON=$(jq -r ".\"$TARGET_KEY\"" "$CONFIG_FILE")
[[ "$CONFIG_JSON" != "null" ]] || { write_st_error "Target '$TARGET_KEY' not found"; exit 1; }

EXTRA_ARGS=(
    "-DSTATIONEERS_LANG=zh-hans"
    "-DBUILD_IC10_COMPILER_EXPORTS_JAVA=ON"
)

# Append command-line args passed to this script
EXTRA_ARGS+=("$@")

build_java "$CONFIG_JSON" "${EXTRA_ARGS[@]}"
