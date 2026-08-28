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
 * @date 2026/06/02 23:49
 * @brief IC10 compiler English (US) localization message definitions
 * @details Defines English (US) translations for all IC10 compiler diagnostic messages
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_EN_US_HPP
#define COMPILER_EN_US_HPP
#pragma once

#include "../local.hpp"
#include "common/utils/fstr.hpp"

namespace stationeers::ic10 {

    struct EnUs;

    using DefaultLanguage = EnUs;

    /**
     * @def LOCAL_MSG_EN_US(id, msg)
     * @if zh
     *
     * @brief 定义美式英语本地化消息
     * @details 为美式英语语言定义本地化消息的便捷宏
     * @param id 消息ID
     * @param msg 消息内容
     *
     * @elseif en
     *
     * @brief Define American English localization message
     * @details Convenience macro for defining localization messages for American English
     * @param id Message ID
     * @param msg Message content
     *
     * @endif
     */
#define IC10_COMPILER_LOCAL_MSG_EN_US(id, msg) IC10_COMPILER_LOCAL_MSG(EnUs, id, msg)

    // Inner Error

    IC10_COMPILER_LOCAL_MSG_EN_US(IE0_1, "{}")

    /* Lexer */

    // Warning

    IC10_COMPILER_LOCAL_MSG_EN_US(
        IWL1,
        "The END token was not encountered as expected at the end of the file - lexical analysis."
    )

    IC10_COMPILER_LOCAL_MSG_EN_US(IWL2, "In IC10 syntax, registers only have r0-r15 - lexical")

    IC10_COMPILER_LOCAL_MSG_EN_US(IWL3, "In IC10 syntax, device only has d0-d5 - lexical")

    // Error

    IC10_COMPILER_LOCAL_MSG_EN_US(IEL1_1, "Unknown operation symbol'{}' - lexical analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IEL2_1, "Unclosed paired symbols '{}' - lexical analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(
        IEL3_1, "Token '{}' is missing a whitespace separator after it - lexical analysis."
    )

    /* Parser */

    // Message

    IC10_COMPILER_LOCAL_MSG_EN_US(IMP1, "Unexpectedly encountered the end of the file - syntax analysis.")

    // Error

    IC10_COMPILER_LOCAL_MSG_EN_US(IEP1_1, "Unexpected token '{}' - syntax analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(
        IEP3_1,
        "Expected to read a executable instruction token, but a '{}' token was read instead "
        "- syntax analysis."
    )

    IC10_COMPILER_LOCAL_MSG_EN_US(IEP23, "Missing colon after label - syntax analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IEP26, "Missing newline between statements - syntax analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IEP27, "Invalid doc tag - syntax analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IEP28, "Unclosed @device block, expected @end-device - syntax analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IEP29, "Unclosed @enum block, expected @end-enum - syntax analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IEP30, "Expected to read @device or @enum token - syntax analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IEP31, "Non-builtin constant cannot be a String type - syntax analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IEP32_1, "Unknown builtin constant '{}' - syntax analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IEP33_1, "Request but did not find the expected '{}' tag - syntax analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IEP34_1, "Not one of the types in '{}' - syntax analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IEP35, "General registers are not within the range of r0-r17 - syntax analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IEP36, "Ordinary device are not within the range of r0-r5 - syntax analysis.")

    /* Analysis */

    // Warning

    IC10_COMPILER_LOCAL_MSG_EN_US(IWA1_1, "'{}' is not a Register type - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IWA2_1, "'{}' is not a Device type - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IWA3_1, "'{}' is not a Register or Number type - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IWA4_1, "'{}' is not a Device or Register type - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IWA5_1, "'{}' is not a LogicSlot type - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IWA6_1, "'{}' is not a ReagentMode type - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IWA7_1, "'{}' is not a Label, Number or Register type - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IWA8_1, "'{}' is not a Logic type - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IWA9_1, "'{}' is not a SlotIndex type - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IWA10_1, "'{}' is not a BatchMode type - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IWA11_2, "'{}' is not within the LogicSlot of device '{}' - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IWA12_1, "'{}' is not a known LogicSlot - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IWA13_1, "'{}' is not a known ReagentMode - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IWA14_2, "'{}' is not within the Logic of device '{}' - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IWA15_1, "'{}' is not a known Logic - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IWA16_2, "'{}' is not within the Slot of device '{}' - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IWA17_1, "'{}' is not a known BatchMode - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IWA18_1, "'{}' is not a Register or Device type - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IWA19_1, "'{}' is not an Address type - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IWA20_1, "'{}' is not a HardwareId type - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IWA21_1, "'{}' is not a ReagentHash type - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IWA22_1, "'{}' is not a DeviceHash type - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IWA23_1, "'{}' is not a NameHash type - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IWA24, "Deprecated syntax - semantic analysis.")

    // Error

    IC10_COMPILER_LOCAL_MSG_EN_US(IEA1_2, "'{}' expected, but found '{}' - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IEA2_1, "The symbol '{}' has been redefined - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IEA3_1, "Identifier '{}' has not been defined - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IEA4, "An alias cannot be defined for another alias - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IEA6, "Unknown Grammar Type '{}' - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IEA7_1, "The Device '{}' should not be annotated as an Enum type - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IEA8_1, "Missing standard library type '{}' - semantic analysis.")

    IC10_COMPILER_LOCAL_MSG_EN_US(IEA9, "SlotIndex type cannot be negative - semantic analysis.")

    /* Complier */

    // Error

    IC10_COMPILER_LOCAL_MSG_EN_US(IIO0_1, R"(ic10c - IC10 compiler

Usage: {}

[options] <input-file>...
       [options] --link <input-file>...

Options:
  -h, --help           Show this help message and exit
  -v, --version        Show version information and exit
  -o, --output FILE    Write output to FILE instead of stdout

Stage selection:
  --emit-tokens        Output token stream as JSON and exit
  --emit-ast           Output AST as JSON and exit
  --emit-symbols       Perform semantic analysis and output symbol table as JSON
                       (default if no --emit-* option is specified)
  --link               Link multiple compilation units and output the merged
                       symbol table. Multiple input files are allowed; ignores
                       --emit-tokens/--emit-ast and forces symbol-table output.

Output format:
  --pretty             Pretty-print JSON output

Localization:
  --locale LANG        Set locale: zh-hans, en-us (default: en-us)

Exit codes:
  0  Success
  1  Command-line or file error
  2  Compilation produced diagnostics (warnings or errors))")

    IC10_COMPILER_LOCAL_MSG_EN_US(IIO1_1, "option '{}' requires parameters")

    IC10_COMPILER_LOCAL_MSG_EN_US(IIO2_1, "unknown option '{}'")

    IC10_COMPILER_LOCAL_MSG_EN_US(IIO3_1, "excess input files '{}'")

    IC10_COMPILER_LOCAL_MSG_EN_US(IIO4, "missing input file")

    IC10_COMPILER_LOCAL_MSG_EN_US(IIO5_1, "Error: {}\nUse -h or --help to view help information\n")

    IC10_COMPILER_LOCAL_MSG_EN_US(IIO6_1, "Error: Unsupported languages {}\n")

}  // namespace stationeers::ic10

#endif  // COMPILER_EN_US_HPP
