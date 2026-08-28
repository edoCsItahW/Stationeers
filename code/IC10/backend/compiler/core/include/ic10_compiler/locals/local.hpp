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
    ///< @if zh @brief {}
        ///< @else {} @endif
        IE0_1,

        /* 词法分析阶段 (Lexer) */

        ///< @if zh @brief 文件结束时未如期遇到END令牌 - 词法分析器。
        ///< @else The END token was not encountered as expected at the end of the file - lexical analysis. @endif
        IWL1,

        ///< @if zh @brief 在IC10语法中，寄存器只有r0-r15 - 词法分析器。
        ///< @else In IC10 syntax, registers only have r0-r15 - lexical @endif
        IWL2,

        ///< @if zh @brief 在IC10语法中，设备只有d0-d5 - 词法分析器。
        ///< @else In IC10 syntax, device only has d0-d5 - lexical @endif
        IWL3,

        ///< @if zh @brief 未知操作符号'{}' - 词法分析器。
        ///< @else Unknown operation symbol'{}' - lexical analysis. @endif
        IEL1_1,

        ///< @if zh @brief 未闭合的成对符号'{}' - 词法分析器。
        ///< @else Unclosed paired symbols '{}' - lexical analysis. @endif
        IEL2_1,

        ///< @if zh @brief 令牌'{}'后缺少空白字符分隔 - 词法分析器。
        ///< @else Token '{}' is missing a whitespace separator after it - lexical analysis. @endif
        IEL3_1,

        /* 语法分析阶段 (Parser) */

        ///< @if zh @brief 意外的遇到文件结尾 - 语法分析器。
        ///< @else Unexpectedly encountered the end of the file - syntax analysis. @endif
        IMP1,

        ///< @if zh @brief 意外的'{}'令牌 - 语法分析器。
        ///< @else Unexpected token '{}' - syntax analysis. @endif
        IEP1_1,

        ///< @if zh @brief 期望读取可执行指令令牌，结果读到'{}'令牌 - 语法分析器。
        ///< @else Expected to read a executable instruction token, but a '{}' token was read instead - syntax analysis. @endif
        IEP3_1,

        ///< @if zh @brief 标签后缺少冒号 - 语法分析器。
        ///< @else Missing colon after label - syntax analysis. @endif
        IEP23,

        ///< @if zh @brief 语句之间缺少换行符 - 语法分析器。
        ///< @else Missing newline between statements - syntax analysis. @endif
        IEP26,

        ///< @if zh @brief 无效的文档标记 - 语法分析器。
        ///< @else Invalid doc tag - syntax analysis. @endif
        IEP27,

        ///< @if zh @brief 未闭合的@device块，应为@end-device - 语法分析。
        ///< @else Unclosed @device block, expected @end-device - syntax analysis. @endif
        IEP28,

        ///< @if zh @brief 未闭合的@enum块，应为@end-enum - 语法分析。
        ///< @else Unclosed @enum block, expected @end-enum - syntax analysis. @endif
        IEP29,

        ///< @if zh @brief 预期读取到@device或@enum标记 - 语法分析。
        ///< @else Expected to read @device or @enum token - syntax analysis. @endif
        IEP30,

        ///< @if zh @brief 非内置常量不允许为String类型 - 语法分析器。
        ///< @else Non-builtin constant cannot be a String type - syntax analysis. @endif
        IEP31,

        ///< @if zh @brief 未知的内置常量'{}' - 语法分析器。
        ///< @else Unknown builtin constant '{}' - syntax analysis. @endif
        IEP32_1,

        ///< @if zh @brief 要求却未找到预期的'{}'标记 - 语法分析器。
        ///< @else Request but did not find the expected '{}' tag - syntax analysis. @endif
        IEP33_1,

        ///< @if zh @brief 不是'{}'中的任何一种类型 - 语法分析器。
        ///< @else Not one of the types in '{}' - syntax analysis. @endif
        IEP34_1,

        ///< @if zh @brief 通用寄存器不在r0-r17范围内 - 语法分析。
        ///< @else General registers are not within the range of r0-r17 - syntax analysis. @endif
        IEP35,

        ///< @if zh @brief 平凡设备不在r0-r5范围内 - 语法分析。
        ///< @else Ordinary device are not within the range of r0-r5 - syntax analysis. @endif
        IEP36,

        /* 语义分析阶段 (Analysis) */

        ///< @if zh @brief '{}'不是一个寄存器类型 - 语义分析。
        ///< @else '{}' is not a Register type - semantic analysis. @endif
        IWA1_1,

        ///< @if zh @brief '{}'不是一个设备类型 - 语义分析。
        ///< @else '{}' is not a Device type - semantic analysis. @endif
        IWA2_1,

        ///< @if zh @brief '{}'不是一个寄存器或数值类型 - 语义分析。
        ///< @else '{}' is not a Register or Number type - semantic analysis. @endif
        IWA3_1,

        ///< @if zh @brief '{}'不是一个设备或寄存器类型 - 语义分析。
        ///< @else '{}' is not a Device or Register type - semantic analysis. @endif
        IWA4_1,

        ///< @if zh @brief '{}'不是一个逻辑槽（LogicSlot）类型 - 语义分析。
        ///< @else '{}' is not a LogicSlot type - semantic analysis. @endif
        IWA5_1,

        ///< @if zh @brief '{}'不是一个试剂模式（ReagentMode）类型 - 语义分析。
        ///< @else '{}' is not a ReagentMode type - semantic analysis. @endif
        IWA6_1,

        ///< @if zh @brief '{}'不是一个标签、数值或寄存器类型 - 语义分析。
        ///< @else '{}' is not a Label, Number or Register type - semantic analysis. @endif
        IWA7_1,

        ///< @if zh @brief '{}'不是一个逻辑（Logic）类型 - 语义分析。
        ///< @else '{}' is not a Logic type - semantic analysis. @endif
        IWA8_1,

        ///< @if zh @brief '{}'不是一个槽索引（SlotIndex）类型 - 语义分析。
        ///< @else '{}' is not a SlotIndex type - semantic analysis. @endif
        IWA9_1,

        ///< @if zh @brief '{}'不是一个批模式（BatchMode）类型 - 语义分析。
        ///< @else '{}' is not a BatchMode type - semantic analysis. @endif
        IWA10_1,

        ///< @if zh @brief '{}'不在设备'{}'的LogicSlot之内 - 语义分析。
        ///< @else '{}' is not within the LogicSlot of device '{}' - semantic analysis. @endif
        IWA11_2,

        ///< @if zh @brief '{}'不是已知的LogicSlot - 语义分析。
        ///< @else '{}' is not a known LogicSlot - semantic analysis. @endif
        IWA12_1,

        ///< @if zh @brief '{}'不是已知的ReagentMode - 语义分析。
        ///< @else '{}' is not a known ReagentMode - semantic analysis. @endif
        IWA13_1,

        ///< @if zh @brief '{}'不在设备'{}'的Logic之内 - 语义分析。
        ///< @else '{}' is not within the Logic of device '{}' - semantic analysis. @endif
        IWA14_2,

        ///< @if zh @brief '{}'不是已知的Logic - 语义分析。
        ///< @else '{}' is not a known Logic - semantic analysis. @endif
        IWA15_1,

        ///< @if zh @brief '{}'不在设备'{}'的Slot之内 - 语义分析。
        ///< @else '{}' is not within the Slot of device '{}' - semantic analysis. @endif
        IWA16_2,

        ///< @if zh @brief '{}'不是已知的BatchMode - 语义分析。
        ///< @else '{}' is not a known BatchMode - semantic analysis. @endif
        IWA17_1,

        ///< @if zh @brief '{}'不是一个寄存器或设备类型 - 语义分析。
        ///< @else '{}' is not a Register or Device type - semantic analysis. @endif
        IWA18_1,

        ///< @if zh @brief '{}'不是一个地址类型 - 语义分析。
        ///< @else '{}' is not an Address type - semantic analysis. @endif
        IWA19_1,

        ///< @if zh @brief '{}'不是一个硬件ID类型 - 语义分析。
        ///< @else '{}' is not a HardwareId type - semantic analysis. @endif
        IWA20_1,

        ///< @if zh @brief '{}'不是一个试剂哈希类型 - 语义分析。
        ///< @else '{}' is not a ReagentHash type - semantic analysis. @endif
        IWA21_1,

        ///< @if zh @brief '{}'不是一个设备哈希类型 - 语义分析。
        ///< @else '{}' is not a DeviceHash type - semantic analysis. @endif
        IWA22_1,

        ///< @if zh @brief '{}'不是一个名称哈希类型 - 语义分析。
        ///< @else '{}' is not a NameHash type - semantic analysis. @endif
        IWA23_1,

        ///< @if zh @brief 已废弃的语法 - 语义分析。
        ///< @else Deprecated syntax - semantic analysis. @endif
        IWA24,

        ///< @if zh @brief 期望'{}'，结果找到'{}' - 语义分析。
        ///< @else '{}' expected, but found '{}' - semantic analysis. @endif
        IEA1_2,

        ///< @if zh @brief 符号'{}'被重新定义了 - 语义分析。
        ///< @else The symbol '{}' has been redefined - semantic analysis. @endif
        IEA2_1,

        ///< @if zh @brief 未找到符号'{}' - 语义分析。
        ///< @else Identifier '{}' has not been defined - semantic analysis. @endif
        IEA3_1,

        ///< @if zh @brief 不能对别名定义别名 - 语义分析。
        ///< @else An alias cannot be defined for another alias - semantic analysis. @endif
        IEA4,

        ///< @if zh @brief 未知语法类型'{}' - 语义分析。
        ///< @else Unknown Grammar Type '{}' - semantic analysis. @endif
        IEA6,

        ///< @if zh @brief 设备'{}'不应被注释为Enum类型 - 语义分析。
        ///< @else The Device '{}' should not be annotated as an Enum type - semantic analysis. @endif
        IEA7_1,

        ///< @if zh @brief 缺失的标准库类型'{}' - 语义分析。
        ///< @else Missing standard library type '{}' - semantic analysis. @endif
        IEA8_1,

        ///< @if zh @brief SlotIndex类型不允许为负数 - 语义分析。
        ///< @else SlotIndex type cannot be negative - semantic analysis. @endif
        IEA9,

        /* 编译器命令行阶段 (Compiler IO) */

        ///< @if zh @brief ic10c - IC10编译器 用法: {} [选项...] <输入文件>...        [选项...] --link <输入文件>... 选项:   -h, --help           显示此帮助消息并退出   -v, --version        显示版本信息并退出   -o, --output FILE    将输出写入 FILE 而非 stdout 阶段选择:   --emit-tokens        将词法令牌流输出为JSON并退出   --emit-ast           将AST（抽象语法数）输出为JSON并退出   --emit-symbols       执行语义分析并将符号表输出为JSON                        (如果未指定 --emit-* 选项，则默认此设置)   --link               链接多个编译单元并输出合并后的符号表。                        允许指定多个输入文件；忽略 --emit-tokens/--emit-ast，                        强制输出符号表。 输出格式化:   --pretty             美观的输出JSON 本地化:   --locale LANG        设置本地化文本: zh-hans, en-us (默认: en-us) 退出码:   0  成功   1  命令行或文件错误   2  编译器生成的诊断 (警告或错误)
        ///< @else ic10c - IC10 compiler Usage: {} [options] <input-file>...        [options] --link <input-file>... Options:   -h, --help           Show this help message and exit   -v, --version        Show version information and exit   -o, --output FILE    Write output to FILE instead of stdout Stage selection:   --emit-tokens        Output token stream as JSON and exit   --emit-ast           Output AST as JSON and exit   --emit-symbols       Perform semantic analysis and output symbol table as JSON                        (default if no --emit-* option is specified)   --link               Link multiple compilation units and output the merged                        symbol table. Multiple input files are allowed; ignores                        --emit-tokens/--emit-ast and forces symbol-table output. Output format:   --pretty             Pretty-print JSON output Localization:   --locale LANG        Set locale: zh-hans, en-us (default: en-us) Exit codes:   0  Success   1  Command-line or file error   2  Compilation produced diagnostics (warnings or errors) @endif
        IIO0_1,

        ///< @if zh @brief 选项'{}'需要参数
        ///< @else option '{}' requires parameters @endif
        IIO1_1,

        ///< @if zh @brief 未知的选项'{}'
        ///< @else unknown option '{}' @endif
        IIO2_1,

        ///< @if zh @brief 多余的输入文件'{}'
        ///< @else excess input files '{}' @endif
        IIO3_1,

        ///< @if zh @brief 缺少输入文件
        ///< @else missing input file @endif
        IIO4,

        ///< @if zh @brief 错误: {} 使用 -h 或 --help 查看帮助信息
        ///< @else Error: {} Use -h or --help to view help information  @endif
        IIO5_1,

        ///< @if zh @brief 错误: 不支持的语言{}
        ///< @else Error: Unsupported languages {} @endif
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
