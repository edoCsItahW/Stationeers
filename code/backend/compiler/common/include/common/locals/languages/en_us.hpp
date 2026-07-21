/**
 * @file en_us.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/19 10:42
 * @if zh
 * @brief 美式英语本地化实现
 * @details 提供公共模块的美式英语消息定义,使用COMMON_LOCAL_MSG_EN_US宏注册消息。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief American English localization implementation
 * @details Provides American English message definitions for common modules,
 *        using COMMON_LOCAL_MSG_EN_US macro to register messages.
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef COMPILER_COMMON_EN_US_HPP
#define COMPILER_COMMON_EN_US_HPP
#pragma once

#include "../common_local.hpp"
#include "common/utils/fstr.hpp"

namespace stationeers {

    /**
     * @if zh
     * @brief 美式英语语言标记结构
     * @elseif en
     * @brief American English language tag struct
     * @endif
     */
    struct EnUs;

    /**
     * @if zh
     * @brief 美式英语本地化消息宏
     * @elseif en
     * @brief American English localization message macro
     * @endif
     */
#define COMMON_LOCAL_MSG_EN_US(id, msg) COMMON_LOCAL_MSG(EnUs, id, msg)

    COMMON_LOCAL_MSG_EN_US(CEF1_1, "could not open file '{}' - file system.")

    COMMON_LOCAL_MSG_EN_US(CEF2_1, "failed to read file '{}' - file system.")

    COMMON_LOCAL_MSG_EN_US(CEF3_1, "unable to write to file '{}' - file system.")

}  // namespace stationeers


#endif  // COMPILER_COMMON__EN_US_HPP