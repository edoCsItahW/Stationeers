/**
 * @file zh_hans.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/19 10:42
 * @if zh
 * @brief 简体中文本地化实现
 * @details 提供公共模块的简体中文消息定义,使用COMMON_LOCAL_MSG_ZH_HANS宏注册消息。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Simplified Chinese localization implementation
 * @details Provides simplified Chinese message definitions for common modules,
 *        using COMMON_LOCAL_MSG_ZH_HANS macro to register messages.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef COMPILER_COMMON_ZH_HANS_HPP
#define COMPILER_COMMON_ZH_HANS_HPP
#pragma once

#include "../common_local.hpp"
#include "common/utils/fstr.hpp"

namespace stationeers {

    /**
     * @if zh
     * @brief 简体中文语言标记结构
     * @elseif en
     * @brief Simplified Chinese language tag struct
     * @endif
     */
    struct ZhHans;

    /**
     * @if zh
     * @brief 简体中文本地化消息宏
     * @elseif en
     * @brief Simplified Chinese localization message macro
     * @endif
     */
#define COMMON_LOCAL_MSG_ZH_HANS(id, msg) COMMON_LOCAL_MSG(ZhHans, id, msg)

    COMMON_LOCAL_MSG_ZH_HANS(CEF1_1, "无法打开文件'{}' - 文件系统。")

    COMMON_LOCAL_MSG_ZH_HANS(CEF2_1, "读取文件'{}'失败 - 文件系统。")

    COMMON_LOCAL_MSG_ZH_HANS(CEF3_1, "无法写入文件'{}' - 文件系统。")

}  // namespace stationeers

#endif  // COMPILER_COMMON_ZH_HANS_HPP