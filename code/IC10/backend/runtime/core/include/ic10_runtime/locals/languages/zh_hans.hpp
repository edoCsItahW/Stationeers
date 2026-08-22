// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file zh_hans.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/09 15:23
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef IC10_RUNTIME_ZH_HANS_HPP
#define IC10_RUNTIME_ZH_HANS_HPP
#pragma once

#include "common/utils/fstr.hpp"
#include "../local.hpp"

namespace stationeers::ic10 {

    struct ZhHans;

#define IC10_RUNTIME_LOCAL_MSG_ZH_HANS(id, msg) IC10_RUNTIME_LOCAL_MSG(ZhHans, id, msg)

    IC10_RUNTIME_LOCAL_MSG_ZH_HANS(IEM0, "栈上溢出错误。")

    IC10_RUNTIME_LOCAL_MSG_ZH_HANS(IEM1, "栈下溢出错误。")

    IC10_RUNTIME_LOCAL_MSG_ZH_HANS(IEM2_1, "无法计算操作数'{}'。")

    IC10_RUNTIME_LOCAL_MSG_ZH_HANS(IEM3, "索引超出范围。")

    IC10_RUNTIME_LOCAL_MSG_ZH_HANS(IEC4, "存在错误语句。")

    IC10_RUNTIME_LOCAL_MSG_ZH_HANS(IEE1_1, "无法找到位于行'{}'的PC。")

    IC10_RUNTIME_LOCAL_MSG_ZH_HANS(IEE2_1, "内部错误'{}'。")

    IC10_RUNTIME_LOCAL_MSG_ZH_HANS(IEE3, "未知错误。")

    IC10_RUNTIME_LOCAL_MSG_ZH_HANS(IEC1, "除零错误。")

    IC10_RUNTIME_LOCAL_MSG_ZH_HANS(IEC2_1, "无效的配置参数'{}'。")

}  // namespace stationeers::ic10

#endif  // IC10_RUNTIME_ZH_HANS_HPP
