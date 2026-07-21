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
 * @date 2026/06/03 11:45
 * @brief IC10编译器中文(简体)本地化消息定义
 * @details 定义IC10编译器所有诊断消息的中文(简体)翻译
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
#define IC10_LOCAL_MSG_ZH_HANS(id, msg) IC10_LOCAL_MSG(ZhHans, id, msg)

    // Inner Error

    IC10_LOCAL_MSG_ZH_HANS(IE0_1, "{}")

    /* Lexer */

    // Warnning

    IC10_LOCAL_MSG_ZH_HANS(IWL1, "文件结束时未如期遇到END令牌 - 词法分析器。");

    IC10_LOCAL_MSG_ZH_HANS(IWL2, "在IC10语法中，寄存器只有r0-r15 - 词法分析器。")

    IC10_LOCAL_MSG_ZH_HANS(IWL3, "在IC10语法中，设备只有d0-d5 - 词法分析器。")

    // Error

    IC10_LOCAL_MSG_ZH_HANS(IEL1_1, "未知操作符号'{}' - 词法分析器。")

    IC10_LOCAL_MSG_ZH_HANS(IEL2_1, "未闭合的成对符号'{}' - 词法分析器。")

    IC10_LOCAL_MSG_ZH_HANS(IEL3_2, "令牌'{}'后紧随'{}'，缺少空白字符分隔 - 词法分析器。")

    /* Parser */

    // Message

    IC10_LOCAL_MSG_ZH_HANS(IMP1, "意外的遇到文件结尾 - 语法分析器。")

    // Error

    IC10_LOCAL_MSG_ZH_HANS(IEP1_1, "意外的'{}'令牌 - 语法分析器。")

    IC10_LOCAL_MSG_ZH_HANS(IEP2_1, "期望读取预处理指令令牌，结果读到'{}'令牌 - 语法分析器。")

    IC10_LOCAL_MSG_ZH_HANS(IEP3_1, "期望读取可执行指令令牌，结果读到'{}'令牌 - 语法分析器。")

    IC10_LOCAL_MSG_ZH_HANS(IEP4_1, "期望读取一元指令令牌，结果读到'{}'令牌 - 语法分析器。")

    IC10_LOCAL_MSG_ZH_HANS(IEP5_1, "期望读取二元指令令牌，结果读到'{}'令牌 - 语法分析器。")

    IC10_LOCAL_MSG_ZH_HANS(IEP6_1, "期望读取三元指令令牌，结果读到'{}'令牌 - 语法分析器。")

    IC10_LOCAL_MSG_ZH_HANS(IEP7_1, "期望读取四元指令令牌，结果读到'{}'令牌 - 语法分析器。")

    IC10_LOCAL_MSG_ZH_HANS(IEP8_1, "期望读取五元指令令牌，结果读到'{}'令牌 - 语法分析器。")

    IC10_LOCAL_MSG_ZH_HANS(IEP9_1, "期望读取六元指令令牌，结果读到'{}'令牌 - 语法分析器。")

    IC10_LOCAL_MSG_ZH_HANS(IEP10_1, "期望读取操作数令牌，结果读到'{}'令牌 - 语法分析器。")

    IC10_LOCAL_MSG_ZH_HANS(IEP11_1, "期望读取寄存器或设备令牌，结果读到'{}'令牌 - 语法分析器。")

    IC10_LOCAL_MSG_ZH_HANS(IEP12_1, "期望读取寄存器或标识符令牌，结果读到'{}'令牌 - 语法分析器。")

    IC10_LOCAL_MSG_ZH_HANS(IEP13_1, "期望读取设备引用令牌，结果读到'{}'令牌 - 语法分析器。")

    IC10_LOCAL_MSG_ZH_HANS(IEP14_1, "期望读取宏调用令牌，结果读到'{}'令牌 - 语法分析器。")

    IC10_LOCAL_MSG_ZH_HANS(IEP15_1, "期望读取标识符或数字令牌，结果读到'{}'令牌 - 语法分析器。")

    IC10_LOCAL_MSG_ZH_HANS(IEP16_1, "期望读取数字令牌，结果读到'{}'令牌 - 语法分析器。")

    IC10_LOCAL_MSG_ZH_HANS(IEP17, "缺少操作数 - 语法分析器。")
    IC10_LOCAL_MSG_ZH_HANS(IEP18, "缺少寄存器或设备 - 语法分析器。")
    IC10_LOCAL_MSG_ZH_HANS(IEP19, "缺少寄存器或标识符 - 语法分析器。")
    IC10_LOCAL_MSG_ZH_HANS(IEP20, "缺少设备引用 - 语法分析器。")
    IC10_LOCAL_MSG_ZH_HANS(IEP21, "缺少宏调用 - 语法分析器。")
    IC10_LOCAL_MSG_ZH_HANS(IEP22, "缺少标识符或数字 - 语法分析器。")
    IC10_LOCAL_MSG_ZH_HANS(IEP23, "标签后缺少冒号 - 语法分析器。")
    IC10_LOCAL_MSG_ZH_HANS(IEP24, "缺少 alias 关键字 - 语法分析器。")
    IC10_LOCAL_MSG_ZH_HANS(IEP25, "缺少 define 关键字 - 语法分析器。")

    IC10_LOCAL_MSG_ZH_HANS(IEP26, "语句之间缺少换行符 - 语法分析器。")

    IC10_LOCAL_MSG_ZH_HANS(IEP27, "无效的文档标记 - 语法分析器。")

    IC10_LOCAL_MSG_ZH_HANS(IEP28, "未闭合的@device块，应为@end-device - 语法分析。")

    IC10_LOCAL_MSG_ZH_HANS(IEP29, "未闭合的@enum块，应为@end-enum - 语法分析。")

    IC10_LOCAL_MSG_ZH_HANS(IEP30, "预期读取到@device或@enum标记 - 语法分析。")

    /* Analysis */

    // Warnning

    IC10_LOCAL_MSG_ZH_HANS(IWA1_1, "'{}'不是一个寄存器类型 - 语义分析。")

    IC10_LOCAL_MSG_ZH_HANS(IWA2_1, "'{}'不是一个设备类型 - 语义分析。")

    IC10_LOCAL_MSG_ZH_HANS(IWA3_1, "'{}'不是一个寄存器或数值类型 - 语义分析。")

    IC10_LOCAL_MSG_ZH_HANS(IWA4_1, "'{}'不是一个设备或寄存器类型 - 语义分析。")

    IC10_LOCAL_MSG_ZH_HANS(IWA5_1, "'{}'不是一个逻辑槽（LogicSlot）类型 - 语义分析。")

    IC10_LOCAL_MSG_ZH_HANS(IWA6_1, "'{}'不是一个试剂模式（ReagentMode）类型 - 语义分析。")

    IC10_LOCAL_MSG_ZH_HANS(IWA7_1, "'{}'不是一个标签类型 - 语义分析。")

    IC10_LOCAL_MSG_ZH_HANS(IWA8_1, "'{}'不是一个逻辑（Logic）类型 - 语义分析。")

    IC10_LOCAL_MSG_ZH_HANS(IWA9_1, "'{}'不是一个槽索引（SlotIndex）类型 - 语义分析。")

    IC10_LOCAL_MSG_ZH_HANS(IWA10_1, "'{}'不是一个批模式（BatchMode）类型 - 语义分析。")

    IC10_LOCAL_MSG_ZH_HANS(IWA11_2, "'{}'不在设备'{}'的LogicSlot之内 - 语义分析。")

    IC10_LOCAL_MSG_ZH_HANS(IWA12_1, "'{}'不是已知的LogicSlot - 语义分析。")

    IC10_LOCAL_MSG_ZH_HANS(IWA13_1, "'{}'不是已知的ReagentMode - 语义分析。")

    IC10_LOCAL_MSG_ZH_HANS(IWA14_2, "'{}'不在设备'{}'的Logic之内 - 语义分析。")

    IC10_LOCAL_MSG_ZH_HANS(IWA15_1, "'{}'不是已知的Logic - 语义分析。")

    IC10_LOCAL_MSG_ZH_HANS(IWA16_2, "'{}'不在设备'{}'的Slot之内 - 语义分析。")

    IC10_LOCAL_MSG_ZH_HANS(IWA17_1, "'{}'不是已知的BatchMode - 语义分析。")

    // Error

    IC10_LOCAL_MSG_ZH_HANS(IEA1_2, "期望'{}'，结果找到'{}' - 语义分析。")

    IC10_LOCAL_MSG_ZH_HANS(IEA2_1, "符号'{}'被重新定义了 - 语义分析。")

    IC10_LOCAL_MSG_ZH_HANS(IEA3_1, "未找到符号'{}' - 语义分析。")

    IC10_LOCAL_MSG_ZH_HANS(IEA4, "不能对别名定义别名 - 语义分析。")

    IC10_LOCAL_MSG_ZH_HANS(IEA5_1, "常量'{}'被重新定义 - 语义分析。")

    IC10_LOCAL_MSG_ZH_HANS(IEA6, "未知语法类型'{}' - 语义分析。")

    IC10_LOCAL_MSG_ZH_HANS(IEA7_1, "设备'{}'不应被注释为Enum类型 - 语义分析。")

    IC10_LOCAL_MSG_ZH_HANS(IEA8_1, "缺失的标准库类型'{}' - 语义分析。")

    /* Complier */

    // Error

    IC10_LOCAL_MSG_ZH_HANS(IIO0_1, R"(ic10c - IC10编译器

用法: {}

[选项...] <输入文件>...
       [选项...] --link <输入文件>...

选项:
  -h, --help           显示此帮助消息并退出
  -v, --version        显示版本信息并退出
  -o, --output FILE    将输出写入 FILE 而非 stdout

阶段选择:
  --emit-tokens        将词法令牌流输出为JSON并退出
  --emit-ast           将AST（抽象语法数）输出为JSON并退出
  --emit-symbols       执行语义分析并将符号表输出为JSON
                       (如果未指定 --emit-* 选项，则默认此设置)
  --link               链接多个编译单元并输出合并后的符号表。
                       允许指定多个输入文件；忽略 --emit-tokens/--emit-ast，
                       强制输出符号表。

输出格式化:
  --pretty             美观的输出JSON

本地化:
  --locale LANG        设置本地化文本: zh-hans, en-us (默认: en-us)

退出码:
  0  成功
  1  命令行或文件错误
  2  编译器生成的诊断 (警告或错误))")

    IC10_LOCAL_MSG_ZH_HANS(IIO1_1, "选项'{}'需要参数")

    IC10_LOCAL_MSG_ZH_HANS(IIO2_1, "未知的选项'{}'")

    IC10_LOCAL_MSG_ZH_HANS(IIO3_1, "多余的输入文件'{}'")

    IC10_LOCAL_MSG_ZH_HANS(IIO4, "缺少输入文件 ")

    IC10_LOCAL_MSG_ZH_HANS(IIO5_1, "错误: {}\n使用 -h 或 --help 查看帮助信息\n")

    IC10_LOCAL_MSG_ZH_HANS(IIO6_1, "错误: 不支持的语言{}\n")

}  // namespace stationeers::ic10

#endif  // COMPILER_ZH_HANS_HPP
