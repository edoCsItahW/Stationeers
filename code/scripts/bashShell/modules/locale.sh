#!/usr/bin/env bash

CURRENT_LANG="zh-hans"
FALLBACK_LANG="en-us"

# 加载翻译文件
load_lang() {
    local lang="$1"
    local file="$SCRIPT_DIR/modules/languages/${lang}.sh"
    if [[ -f "$file" ]]; then
        source "$file"
        return 0
    fi
    return 1
}

initialize_locale() {
    local lang="${1:-"zh-hans"}"
    if ! load_lang "$lang"; then
        write_st_warning "Failed to load language '$lang', using fallback '$FALLBACK_LANG'."
        load_lang "$FALLBACK_LANG"
    fi
    CURRENT_LANG="$lang"
}

# 获取翻译文本（支持简单替换）
get_text() {
    local key="$1"
    shift
    local args=("$@")
    # 解析嵌套键（例如 Build.Configure.Head）
    local result="$TRANSLATIONS"
    IFS='.' read -ra parts <<< "$key"
    for part in "${parts[@]}"; do
        result=$(echo "$result" | jq -r ".\"$part\" // empty" 2>/dev/null)
        if [[ -z "$result" ]]; then
            # 若未找到，返回原键
            echo "$key"
            return 1
        fi
    done

    # 替换占位符 {0} {1} ...
    local counter=0
    for arg in "${args[@]}"; do
        result="${result//\{$counter\}/$arg}"
        ((counter++))
    done
    echo "$result"
}