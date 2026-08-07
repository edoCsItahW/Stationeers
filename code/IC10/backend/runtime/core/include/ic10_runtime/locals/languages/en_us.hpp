// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file en_us.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/08 14:05
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef IC10_RUNTIME_EN_US_HPP
#define IC10_RUNTIME_EN_US_HPP
#pragma once

#include "../local.hpp"
#include "common/utils/file.hpp"

namespace stationeers::ic10 {

    struct EnUs;

    using DefaultLanguage = EnUs;

#define IC10_RUNTIME_LOCAL_MSG_EN_US(id, msg) IC10_RUNTIME_LOCAL_MSG(EnUs, id, msg)

    IC10_RUNTIME_LOCAL_MSG_EN_US(IEM0, "Stack Overflow Error.")

    IC10_RUNTIME_LOCAL_MSG_EN_US(IEM1, "Stack Underflow Error.")

    IC10_RUNTIME_LOCAL_MSG_EN_US(IEM2_1, "Unable to evaluate operand '{}'.")

    IC10_RUNTIME_LOCAL_MSG_EN_US(IEM3_1, "Index out of range.")

    IC10_RUNTIME_LOCAL_MSG_EN_US(IEC4, "There are incorrect statements present.")

}  // namespace stationeers::ic10

#endif  // IC10_RUNTIME_EN_US_HPP
