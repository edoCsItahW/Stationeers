/**
 * @file zh_hans.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/07/19 10:42
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_COMMON_ZH_HANS_HPP
#define COMPILER_COMMON_ZH_HANS_HPP
#pragma once

#include "../common_local.hpp"
#include "common/utils/fstr.hpp"

namespace stationeers {

    struct ZhHans;

#define COMMON_LOCAL_MSG_ZH_HANS(id, msg) COMMON_LOCAL_MSG(ZhHans, id, msg)

    COMMON_LOCAL_MSG_ZH_HANS(CEF1_1, "无法打开文件'{}' - 文件系统。")

    COMMON_LOCAL_MSG_ZH_HANS(CEF2_1, "读取文件'{}'失败 - 文件系统。")

    COMMON_LOCAL_MSG_ZH_HANS(CEF3_1, "无法写入文件'{}' - 文件系统。")

}  // namespace stationeers

#endif  // COMPILER_COMMON_ZH_HANS_HPP
