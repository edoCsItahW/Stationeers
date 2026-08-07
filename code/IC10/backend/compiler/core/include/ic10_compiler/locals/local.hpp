// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file local.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/02 23:28
 * @if zh
 * @brief IC10本地化消息系统
 * @details 定义IC10编译器的诊断消息ID枚举、消息打包类型及本地化消息注册宏。
 *          支持中英文双语消息，通过模板特化实现类型安全的消息映射。
 *
 * @note 消息ID遵循严格的命名规范：I(IC10) + (E|W|M)(Error|Warning|Message) + 阶段(L|P|A) + 编号
 *
 * @par 消息ID命名规范:
 * @verbatim
 * I(IC10)
 * + (E(Error) | W(Warning) | M(Message) | I(Info))
 * + (L(Lexer) | P(Parser) | A(Analysis) | O(Compiler IO))
 * + {Number}
 * + '_'
 * + {ArgumentNumber}
 * @endverbatim
 *
 * @par 使用方式:
 * @code
 * // 在语言文件中定义消息
 * IC10_LOCAL_MSG(ZhHans, IEL1_1, "未知操作符 '{}'");
 *
 * // 在编译器中使用消息
 * reporter.errorWith<IC10MsgId::IEL1_1>(pos, operatorName);
 * @endcode
 *
 * @see common/locals/local.hpp 基础本地化框架
 * @see DiagnosticReporter 诊断报告器
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief IC10 localization message system
 * @details Defines diagnostic message ID enum, message pack type, and localization
 *          message registration macros for the IC10 compiler. Supports Chinese and
 *          English bilingual messages through template specialization for type-safe
 *          message mapping.
 *
 * @note Message IDs follow strict naming convention: I(IC10) + (E|W|M)(Error|Warning|Message) + Phase(L|P|A) + Number
 *
 * @par Message ID Naming Convention:
 * @verbatim
 * I(IC10)
 * + (E(Error) | W(Warning) | M(Message) | I(Info))
 * + (L(Lexer) | P(Parser) | A(Analysis) | O(Compiler IO))
 * + {Number}
 * + '_'
 * + {ArgumentNumber}
 * @endverbatim
 *
 * @par Usage:
 * @code
 * // Define message in language file
 * IC10_LOCAL_MSG(ZhHans, IEL1_1, "Unknown operator '{}'");
 *
 * // Use message in compiler
 * reporter.errorWith<IC10MsgId::IEL1_1>(pos, operatorName);
 * @endcode
 *
 * @see common/locals/local.hpp Base localization framework
 * @see DiagnosticReporter Diagnostic reporter
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef COMPILER_IC10_LOCAL_HPP
#define COMPILER_IC10_LOCAL_HPP
#pragma once

#include "common/locals/local.hpp"

namespace stationeers::ic10 {

    /**
     * @if zh
     * @enum IC10CompilerMsgId
     * @brief IC10编译器诊断消息ID枚举
     * @details 定义了所有编译器诊断消息的唯一标识符，遵循严格的命名规范。
     *          每个消息ID对应一个可本地化的消息模板。
     *
     * @elseif en
     * @enum IC10CompilerMsgId
     * @brief IC10 compiler diagnostic message ID enum
     * @details Defines unique identifiers for all compiler diagnostic messages,
     *          following a strict naming convention. Each message ID corresponds
     *          to a localizable message template.
     *
     * @endif
     */
    enum class IC10CompilerMsgId : uint32_t {
        /** @if zh @brief 内部错误占位符 @else @brief Internal error placeholder @endif */
        IE0_1,

        /* 词法分析阶段 (Lexer) */

        /** @if zh @brief 文件末尾未预期地遇到END标记 @else @brief The END token was not encountered as expected at the end of the file @endif */
        IWL1,

        /** @if zh @brief 寄存器名超出范围(r0-r15) @else @brief In IC10 syntax, registers only have r0-r15 @endif */
        IWL2,

        /** @if zh @brief 设备名超出范围(d0-d5) @else @brief In IC10 syntax, device only has d0-d5 @endif */
        IWL3,

        /** @if zh @brief 未知操作符 @else @brief Unknown operation symbol @endif */
        IEL1_1,

        /** @if zh @brief 未闭合的配对符号 @else @brief Unclosed paired symbols @endif */
        IEL2_1,

        /** @if zh @brief 两个标记之间缺少空白字符 @else @brief Token immediately followed by another token without whitespace @endif */
        IEL3_2,

        /* 语法分析阶段 (Parser) */

        /** @if zh @brief 意外地遇到文件末尾 @else @brief Unexpectedly encountered the end of the file @endif */
        IMP1,

        /** @if zh @brief 意外的标记 @else @brief Unexpected token @endif */
        IEP1_1,

        /** @if zh @brief 预期预处理指令标记 @else @brief Expected preprocessing directive token @endif */
        IEP2_1,

        /** @if zh @brief 预期可执行指令标记 @else @brief Expected executable instruction token @endif */
        IEP3_1,

        /** @if zh @brief 预期一元指令标记 @else @brief Expected unary instruction token @endif */
        IEP4_1,

        /** @if zh @brief 预期二元指令标记 @else @brief Expected binary instruction token @endif */
        IEP5_1,

        /** @if zh @brief 预期三元指令标记 @else @brief Expected ternary instruction token @endif */
        IEP6_1,

        /** @if zh @brief 预期四元指令标记 @else @brief Expected quaternary instruction token @endif */
        IEP7_1,

        /** @if zh @brief 预期五元指令标记 @else @brief Expected quinary instruction token @endif */
        IEP8_1,

        /** @if zh @brief 预期六元指令标记 @else @brief Expected senary instruction token @endif */
        IEP9_1,

        /** @if zh @brief 预期操作数标记 @else @brief Expected operand tokens @endif */
        IEP10_1,

        /** @if zh @brief 预期寄存器或设备标记 @else @brief Expected register or device token @endif */
        IEP11_1,

        /** @if zh @brief 预期寄存器或标识符标记 @else @brief Expected register or identifier token @endif */
        IEP12_1,

        /** @if zh @brief 预期设备引用标记 @else @brief Expected device reference token @endif */
        IEP13_1,

        /** @if zh @brief 预期宏调用标记 @else @brief Expected macro call token @endif */
        IEP14_1,

        /** @if zh @brief 预期标识符或数字标记 @else @brief Expected identifier or number token @endif */
        IEP15_1,

        /** @if zh @brief 预期数字标记 @else @brief Expected number token @endif */
        IEP16_1,

        /** @if zh @brief 缺少操作数 @else @brief Missing operand @endif */
        IEP17,

        /** @if zh @brief 缺少寄存器或设备 @else @brief Missing register or device @endif */
        IEP18,

        /** @if zh @brief 缺少寄存器或标识符 @else @brief Missing register or identifier @endif */
        IEP19,

        /** @if zh @brief 缺少设备引用 @else @brief Missing device reference @endif */
        IEP20,

        /** @if zh @brief 缺少宏调用 @else @brief Missing macro call @endif */
        IEP21,

        /** @if zh @brief 缺少标识符或数字 @else @brief Missing identifier or number @endif */
        IEP22,

        /** @if zh @brief 标签后缺少冒号 @else @brief Missing colon after label @endif */
        IEP23,

        /** @if zh @brief 缺少alias关键字 @else @brief Missing alias keyword @endif */
        IEP24,

        /** @if zh @brief 缺少define关键字 @else @brief Missing define keyword @endif */
        IEP25,

        /** @if zh @brief 语句之间缺少换行符 @else @brief Missing newline between statements @endif */
        IEP26,

        /** @if zh @brief 无效的文档标签 @else @brief Invalid doc tag @endif */
        IEP27,

        /** @if zh @brief 未闭合的@device块 @else @brief Unclosed @device block @endif */
        IEP28,

        /** @if zh @brief 未闭合的@enum块 @else @brief Unclosed @enum block @endif */
        IEP29,

        /** @if zh @brief 预期@device或@enum标记 @else @brief Expected @device or @enum token @endif */
        IEP30,

        /* 语义分析阶段 (Analysis) */

        /** @if zh @brief 不是Register类型 @else @brief Not a Register type @endif */
        IWA1_1,

        /** @if zh @brief 不是Device类型 @else @brief Not a Device type @endif */
        IWA2_1,

        /** @if zh @brief 不是Register或Number类型 @else @brief Not a Register or Number type @endif */
        IWA3_1,

        /** @if zh @brief 不是Device或Register类型 @else @brief Not a Device or Register type @endif */
        IWA4_1,

        /** @if zh @brief 不是LogicSlot类型 @else @brief Not a LogicSlot type @endif */
        IWA5_1,

        /** @if zh @brief 不是ReagentMode类型 @else @brief Not a ReagentMode type @endif */
        IWA6_1,

        /** @if zh @brief 不是Label、Number或Register类型 @else @brief Not a Label, Number or Register type @endif */
        IWA7_1,

        /** @if zh @brief 不是Logic类型 @else @brief Not a Logic type @endif */
        IWA8_1,

        /** @if zh @brief 不是SlotIndex类型 @else @brief Not a SlotIndex type @endif */
        IWA9_1,

        /** @if zh @brief 不是BatchMode类型 @else @brief Not a BatchMode type @endif */
        IWA10_1,

        /** @if zh @brief 不在设备的LogicSlot范围内 @else @brief Not within the LogicSlot of device @endif */
        IWA11_2,

        /** @if zh @brief 未知的LogicSlot @else @brief Not a known LogicSlot @endif */
        IWA12_1,

        /** @if zh @brief 未知的ReagentMode @else @brief Not a known ReagentMode @endif */
        IWA13_1,

        /** @if zh @brief 不在设备的Logic范围内 @else @brief Not within the Logic of device @endif */
        IWA14_2,

        /** @if zh @brief 未知的Logic @else @brief Not a known Logic @endif */
        IWA15_1,

        /** @if zh @brief 不在设备的Slot范围内 @else @brief Not within the Slot of device @endif */
        IWA16_2,

        /** @if zh @brief 未知的BatchMode @else @brief Not a known BatchMode @endif */
        IWA17_1,

        /** @if zh @brief 类型不匹配 @else @brief Type mismatch @endif */
        IEA1_2,

        /** @if zh @brief 符号已被重定义 @else @brief Symbol has been redefined @endif */
        IEA2_1,

        /** @if zh @brief 标识符未定义 @else @brief Identifier has not been defined @endif */
        IEA3_1,

        /** @if zh @brief 不能为别名定义别名 @else @brief An alias cannot be defined for another alias @endif */
        IEA4,

        /** @if zh @brief 常量已被重定义 @else @brief Constant has been redefined @endif */
        IEA5_1,

        /** @if zh @brief 未知的语法类型 @else @brief Unknown Grammar Type @endif */
        IEA6,

        /** @if zh @brief Device不能被标注为Enum类型 @else @brief Device should not be annotated as an Enum type @endif */
        IEA7_1,

        /** @if zh @brief 缺少标准库类型 @else @brief Missing standard library type @endif */
        IEA8_1,

        /* 编译器命令行阶段 (Compiler IO) */

        /** @if zh @brief 帮助消息 @else @brief Help message @endif */
        IIO0_1,

        /** @if zh @brief 选项需要参数 @else @brief Option requires parameters @endif */
        IIO1_1,

        /** @if zh @brief 未知选项 @else @brief Unknown option @endif */
        IIO2_1,

        /** @if zh @brief 多余的输入文件 @else @brief Excess input files @endif */
        IIO3_1,

        /** @if zh @brief 缺少输入文件 @else @brief Missing input file @endif */
        IIO4,

        /** @if zh @brief 通用错误消息 @else @brief Generic error message @endif */
        IIO5_1,

        /** @if zh @brief 不支持的语言 @else @brief Unsupported languages @endif */
        IIO6_1,

    };

    /**
     * @if zh
     * @struct IC10CompilerMsgMap
     * @brief IC10消息映射模板
     * @details 用于将消息ID和语言类型映射到具体的本地化消息字符串。
     *          通过模板特化在语言文件中定义具体的消息内容。
     *
     * @tparam Lang 语言类型（如ZhHans, EnUs）
     * @tparam MsgId 消息ID枚举值
     *
     * @elseif en
     * @struct IC10CompilerMsgMap
     * @brief IC10 message mapping template
     * @details Maps message ID and language type to specific localized message strings.
     *          Concrete message content is defined in language files through template
     *          specialization.
     *
     * @tparam Lang Language type (e.g., ZhHans, EnUs)
     * @tparam MsgId Message ID enum value
     *
     * @endif
     */
    template<typename, IC10CompilerMsgId>
    struct IC10CompilerMsgMap;

    /**
     * @if zh
     * @typedef IC10MsgPack
     * @brief IC10消息打包类型
     * @details 基于通用MsgPack模板的IC10专用消息打包类型，封装了消息ID类型和消息映射类型。
     *          用于诊断报告器和本地化系统的消息获取。
     *
     * @elseif en
     * @typedef IC10MsgPack
     * @brief IC10 message pack type
     * @details IC10-specific message pack type based on the generic MsgPack template,
     *          encapsulating message ID type and message mapping type. Used by diagnostic
     *          reporter and localization system for message retrieval.
     *
     * @endif
     */
    using IC10CompilerMsgPack = MsgPack<IC10CompilerMsgId, IC10CompilerMsgMap>;

    /**
     * @if zh
     * @class IC10CompilerLocal
     * @brief IC10本地化服务类
     * @details 继承自通用Local模板，提供IC10编译器的本地化消息查询接口。
     *          支持根据消息ID和语言类型获取本地化消息。
     *
     * @elseif en
     * @class IC10CompilerLocal
     * @brief IC10 localization service class
     * @details Inherits from the generic Local template, providing localized message
     *          query interface for the IC10 compiler. Supports retrieving localized
     *          messages based on message ID and language type.
     *
     * @endif
     */
    class IC10CompilerLocal : public Local<IC10CompilerMsgPack> {};

    /**
     * @if zh
     * @typedef IMsgId
     * @brief IC10消息ID的简写
     * @details IC10MsgId的别名，用于简化代码书写。
     *
     * @elseif en
     * @typedef IMsgId
     * @brief Short alias for IC10 message ID
     * @details Alias for IC10MsgId to simplify code writing.
     *
     * @endif
     */
    using ICMsgId = IC10CompilerMsgId;

    /**
     * @if zh
     * @typedef ILoc
     * @brief IC10本地化服务的简写
     * @details IC10Local的别名，用于简化代码书写。
     *
     * @elseif en
     * @typedef ILoc
     * @brief Short alias for IC10 localization service
     * @details Alias for IC10Local to simplify code writing.
     *
     * @endif
     */
    using ICLoc = IC10CompilerLocal;

    /**
     * @def IC10_LOCAL_MSG(type, id, _msg)
     * @if zh
     * @brief 定义本地化消息
     * @details 为指定语言类型和消息ID定义本地化消息模板特化。
     *          使用_fs后缀将字符串字面量转换为fstr类型，支持格式化参数。
     *
     * @param type 语言类型（如ZhHans, EnUs）
     * @param id 消息ID枚举值（不带IC10MsgId::前缀）
     * @param _msg 消息内容字符串，支持{}占位符作为格式化参数
     *
     * @par 示例:
     * @code
     * // 定义中文消息
     * IC10_LOCAL_MSG(ZhHans, IEL1_1, "未知操作符 '{}'");
     * // 定义英文消息
     * IC10_LOCAL_MSG(EnUs, IEL1_1, "Unknown operator '{}'");
     * @endcode
     *
     * @elseif en
     * @brief Define localization message
     * @details Defines localization message template specialization for specified
     *          language type and message ID. Uses _fs suffix to convert string
     *          literal to fstr type, supporting format parameters.
     *
     * @param type Language type (e.g., ZhHans, EnUs)
     * @param id Message ID enum value (without IC10MsgId:: prefix)
     * @param _msg Message content string, supporting {} placeholders for format parameters
     *
     * @par Example:
     * @code
     * // Define Chinese message
     * IC10_LOCAL_MSG(ZhHans, IEL1_1, "Unknown operator '{}'");
     * // Define English message
     * IC10_LOCAL_MSG(EnUs, IEL1_1, "Unknown operator '{}'");
     * @endcode
     *
     * @endif
     */
#define IC10_COMPILER_LOCAL_MSG(type, id, _msg)                                                             \
    template<>                                                                                     \
    struct IC10CompilerMsgMap<type, IC10CompilerMsgId::id> {                                                       \
        static constexpr auto msg = _msg##_fs;                                                     \
    };

}  // namespace stationeers::ic10

#endif  // COMPILER_IC10_LOCAL_HPP
