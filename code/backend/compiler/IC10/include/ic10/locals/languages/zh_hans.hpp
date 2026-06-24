// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file zh_hans.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/03 11:45
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_ZH_HANS_HPP
#define COMPILER_ZH_HANS_HPP
#pragma once

#include "../local.hpp"
#include "common/utils/fstr.hpp"


namespace stationeers::ic10 {

    struct ZhHans;

    /**
     * @def LOCAL_MSG_ZH_HANS(id, msg)
     * @if zh
     *
     * @brief 定义简体中文本地化消息
     * @details 为简体中文语言定义本地化消息的便捷宏
     * @param id 消息ID
     * @param msg 消息内容
     *
     * @elseif en
     *
     * @brief Define Simplified Chinese localization message
     * @details Convenience macro for defining localization messages for Simplified Chinese
     * @param id Message ID
     * @param msg Message content
     *
     * @endif
     */
#define LOCAL_MSG_ZH_HANS(id, msg) LOCAL_MSG(ZhHans, id, msg)

    LOCAL_MSG_ZH_HANS(IWL1, "文件结束时未如期遇到END令牌 - 词法分析器。");

    LOCAL_MSG_ZH_HANS(IEL1_1, "未知操作符号'{}' - 词法分析器。")

    LOCAL_MSG_ZH_HANS(IMP1, "意外的遇到文件结尾 - 语法分析器。")

    LOCAL_MSG_ZH_HANS(IMP2_1, "期望读取预处理指令令牌，结果读到'{}'令牌 - 语法分析器。")

    LOCAL_MSG_ZH_HANS(IMP3_1, "期望读取可执行指令令牌，结果读到'{}'令牌 - 语法分析器。")

    LOCAL_MSG_ZH_HANS(IMP4_1, "期望读取一元指令令牌，结果读到'{}'令牌 - 语法分析器。")

    LOCAL_MSG_ZH_HANS(IMP5_1, "期望读取二元指令令牌，结果读到'{}'令牌 - 语法分析器。")

    LOCAL_MSG_ZH_HANS(IMP6_1, "期望读取三元指令令牌，结果读到'{}'令牌 - 语法分析器。")

    LOCAL_MSG_ZH_HANS(IMP7_1, "期望读取四元指令令牌，结果读到'{}'令牌 - 语法分析器。")

    LOCAL_MSG_ZH_HANS(IMP8_1, "期望读取五元指令令牌，结果读到'{}'令牌 - 语法分析器。")

    LOCAL_MSG_ZH_HANS(IMP9_1, "期望读取六元指令令牌，结果读到'{}'令牌 - 语法分析器。")

    LOCAL_MSG_ZH_HANS(IMP10_1, "期望读取操作数令牌，结果读到'{}'令牌 - 语法分析器。")

    LOCAL_MSG_ZH_HANS(IMP11_1, "期望读取寄存器或设备令牌，结果读到'{}'令牌 - 语法分析器。")

    LOCAL_MSG_ZH_HANS(IMP12_1, "期望读取寄存器或标识符令牌，结果读到'{}'令牌 - 语法分析器。")

    LOCAL_MSG_ZH_HANS(IMP13_1, "期望读取设备引用令牌，结果读到'{}'令牌 - 语法分析器。")

    LOCAL_MSG_ZH_HANS(IMP14_1, "期望读取宏调用令牌，结果读到'{}'令牌 - 语法分析器。")

    LOCAL_MSG_ZH_HANS(IMP15_1, "期望读取标识符或数字令牌，结果读到'{}'令牌 - 语法分析器。")

    LOCAL_MSG_ZH_HANS(IMP16_1, "期望读取数字令牌，结果读到'{}'令牌 - 语法分析器。")

    LOCAL_MSG_ZH_HANS(IEP1_1, "意外的'{}'令牌 - 语法分析器。")

    LOCAL_MSG_ZH_HANS(IEA1_2, "期望'{}'，结果找到'{}' - 语义分析器。")

    LOCAL_MSG_ZH_HANS(IEA2_1, "符号'{}'被重新定义了 - 语义分析器。")

    LOCAL_MSG_ZH_HANS(IEA3_1, "未找到符号'{}' - 语义分析器。")

    LOCAL_MSG_ZH_HANS(IEA4, "不能对别名定义别名 - 语义分析器。")

}  // namespace stationeers::ic10

#endif  // COMPILER_ZH_HANS_HPP
