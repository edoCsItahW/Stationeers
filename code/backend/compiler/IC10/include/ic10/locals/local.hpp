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
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_IC10_LOCAL_HPP
#define COMPILER_IC10_LOCAL_HPP
#pragma once

#include "common/locals/local.hpp"

namespace stationeers::ic10 {

    /**
     * I(IC10)
     * + (E(Error) | W(Warning) | I(Info))
     * + (L(Lexer) | P(Parser) + A(Analysis))
     * + {Number}
     * + '_'
     * + {ArgumentNumber}
     */
    enum class IC10MsgId : uint32_t {
        // Inner Error
        IE0_1,  // '{}'

        /* Lexer */

        // Warnning

        IWL1,  // The END token was not encountered as expected at the end of the file - lexical
               // analysis.
        IWL2,  // In IC10 syntax, registers only have r0-r15 - lexical
        IWL3,  // In IC10 syntax, device only has r0-r5 - lexical

        // Error

        IEL1_1,  // Unknown operation symbol'{}' - lexical analysis.
        IEL2_1,  // Unclosed paired symbols '{}' - lexical analysis.
        IEL3_2,  // Token '{}' is immediately followed by '{}' without whitespace - lexical
                 // analysis.

        /* Parser */

        // Message

        IMP1,  // Unexpectedly encountered the end of the file - syntax analysis.

        // Error

        IEP1_1,  // Unexpected token '{}' - syntax analysis.
        IEP2_1,  // Expected to read a preprocessing directive token, but a '{}' token was read
                 // instead - syntax analysis.
        IEP3_1,  // Expected to read a executable instruction token, but a '{}' token was read
                 // instead
                 // - syntax analysis.
        IEP4_1,  // Expected to read a unary instruction token, but a '{}' token was read
                 // instead - syntax analysis.
        IEP5_1,  // Expected to read a binary instruction token, but a '{}' token was read instead -
                 // syntax analysis.
        IEP6_1,  // Expected to read a ternary instruction token, but a '{}' token was read instead
                 // - syntax analysis.
        IEP7_1,  // Expected to read a quaternary instruction token, but a '{}' token was read
                 // instead
                 // - syntax analysis.
        IEP8_1,  // Expected to read a quinary instruction token, but a '{}' token was read instead
                 // - syntax analysis.
        IEP9_1,  // Expected to read a senary instruction token, but a '{}' token was read instead -
                 // syntax analysis.
        IEP10_1,  // Expected to read a operand tokens, but a '{}' token was read instead - syntax
                  // analysis.
        IEP11_1,  // Expected to read a register or device token, but a '{}' token was read instead
                  // - syntax analysis.
        IEP12_1,  // Expected to read a register or identifier token, but a '{}' token was read
                  // instead - syntax analysis.
        IEP13_1,  // Expected to read the device reference token, but ended up reading the '{}'
                  // token
                  // - syntax analysis.
        IEP14_1,  // Expected to read a macro call token, but a '{}' token was read instead - syntax
                  // analysis.
        IEP15_1,  // Expected to read an identifier or a number token, but a '{}' token was read
                  // instead - syntax analysis.
        IEP16_1,  // Expected to read a number token, but a '{}' token was read instead - syntax
                  // analysis.

        IEP17,  // Missing operand - syntax analysis.
        IEP18,  // Missing register or device - syntax analysis.
        IEP19,  // Missing register or identifier - syntax analysis.
        IEP20,  // Missing device reference - syntax analysis.
        IEP21,  // Missing macro call - syntax analysis.
        IEP22,  // Missing identifier or number - syntax analysis.
        IEP23,  // Missing colon after label - syntax analysis.
        IEP24,  // Missing alias keyword - syntax analysis.
        IEP25,  // Missing define keyword - syntax analysis.
        IEP26,  // Missing newline between statements - syntax analysis.
        IEP27,  // Invalid doc tag - syntax analysis.
        IEP28,  // Unclosed @device block, expected @end-device - syntax analysis
        IEP29,  // Unclosed @enum block, expected @end-enum - syntax analysis
        IEP30,  // Expected to read @device or @enum token - syntax analysis.

        /* Analysis */

        // Warnning

        IWA1_1,   // '{}' is not a Register type - semantic analysis.
        IWA2_1,   // '{}' is not a Device type - semantic analysis.
        IWA3_1,   // '{}' is not a Register or Number type - semantic analysis.
        IWA4_1,   // '{}' is not a Device or Register type - semantic analysis.
        IWA5_1,   // '{}' is not a LogicSlot type - semantic analysis.
        IWA6_1,   // '{}' is not a ReagentMode type - semantic analysis.
        IWA7_1,   // '{}' is not a Label type - semantic analysis.
        IWA8_1,   // '{}' is not a Logic type - semantic analysis.
        IWA9_1,   // '{}' is not a SlotIndex type - semantic analysis.
        IWA10_1,  // '{}' is not a BatchMode type - semantic analysis.
        IWA11_2,  // '{}' is not within the LogicSlot of device '{}' - semantic analysis.
        IWA12_1,  // '{}' is not a known LogicSlot - semantic analysis.
        IWA13_1,  // '{}' is not a known ReagentMode - semantic analysis.
        IWA14_2,  // '{}' is not within the Logic of device '{}' - semantic analysis.
        IWA15_1,  // '{}' is not a known Logic - semantic analysis.
        IWA16_2,  // '{}' is not within the Slot of device '{}' - semantic analysis.
        IWA17_1,  // '{}' is not a known BatchMode - semantic analysis.

        // Error

        IEA1_2,  // '{}' expected, but found '{}' - semantic analysis.
        IEA2_1,  // The symbol '{}' has been redefined - semantic analysis.
        IEA3_1,  // Identifier '{}' has not been defined - semantic analysis.
        IEA4,    // An alias cannot be defined for another alias - semantic analysis.
        IEA5_1,  // Constant '{}' has been redefined - semantic analysis.
        IEA6,    // Unknown Grammar Type '{}' - semantic analysis.
        IEA7_1,  // The Device '{}' should not be annotated as an Enum type - semantic analysis.
        IEA8_1,  // Missing standard library type '{}' - semantic analysis.

        /* Complier */

        // Error

        IIO0_1,  // [ help message ]
        IIO1_1,  // option '{}' requires parameters - IC10 compiler.
        IIO2_1,  // unknown option '{}' - IC10 compiler.
        IIO3_1,  // excess input files '{}' - IC10 compiler.
        IIO4,    // missing input file - IC10 compiler.
        IIO5_1,  // Error: {}\nUse -h or --help to view help information\n
        IIO6_1,  // Error: Unsupported languages {}\n

    };

    template<typename, IC10MsgId>
    struct IC10MsgMap;

    using IC10MsgPack = MsgPack<IC10MsgId, IC10MsgMap>;

    class IC10Local : public Local<IC10MsgPack> {};

    using IMsgId = IC10MsgId;

    using ILoc = IC10Local;

    /**
     * @def LOCAL_MSG(type, id, _msg)
     * @if zh
     *
     * @brief 定义本地化消息
     * @details 为指定语言和消息ID定义本地化消息模板特化
     * @param type 语言类型(如ZhHans, EnUs)
     * @param id 消息ID枚举值
     * @param _msg 消息内容字符串
     *
     * @elseif en
     *
     * @brief Define localization message
     * @details Defines localization message template specialization for specified language and
     * message ID
     * @param type Language type (e.g. ZhHans, EnUs)
     * @param id Message ID enum value
     * @param _msg Message content string
     *
     * @endif
     */
#define IC10_LOCAL_MSG(type, id, _msg)                                                             \
    template<>                                                                                     \
    struct IC10MsgMap<type, IC10MsgId::id> {                                                       \
        static constexpr auto msg = _msg##_fs;                                                     \
    };

}  // namespace stationeers::ic10


#endif  // COMPILER_IC10_LOCAL_HPP
