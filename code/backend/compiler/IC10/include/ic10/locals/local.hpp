// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file local.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/02 23:28
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_LOCAL_HPP
#define COMPILER_LOCAL_HPP
#pragma once

#include "common/locals/local.hpp"

namespace stationeers::ic10 {

    /**
     * I(IC10)
     * + (E(Error) | W(Warning) | I(Info))
     * + (L(Lexer) | P(Parser) + A(Analysis))
     * + {Number}
     */
    enum class IC10MsgId : uint32_t {
        /* Lexer */

        // Warning
        IWL1,  // The END token was not encountered as expected at the end of the file - lexical
               // analysis.

        // Error
        IEL1_1,  // Unknown operation symbol'{}' - lexical analysis.

        /* Parser */

        // Message
        IMP1,    // Unexpectedly encountered the end of the file - syntax analysis.
        IMP2_1,  // Expected to read a preprocessing directive token, but a '{}' token was read
                 // instead - syntax analysis.
        IMP3_1,  // Expected to read a executable instruction token, but a '{}' token was read
                 // instead
                 // - syntax analysis.
        IMP4_1,  // Expected to read a unary instruction token, but a '{}' token was read
                 // instead - syntax analysis.
        IMP5_1,  // Expected to read a binary instruction token, but a '{}' token was read instead -
                 // syntax analysis.
        IMP6_1,  // Expected to read a ternary instruction token, but a '{}' token was read instead
                 // - syntax analysis.
        IMP7_1,  // Expected to read a quaternary instruction token, but a '{}' token was read
                 // instead
                 // - syntax analysis.
        IMP8_1,  // Expected to read a quinary instruction token, but a '{}' token was read instead
                 // - syntax analysis.
        IMP9_1,  // Expected to read a senary instruction token, but a '{}' token was read instead -
                 // syntax analysis.
        IMP10_1,  // Expected to read a operand tokens, but a '{}' token was read instead - syntax
                  // analysis.
        IMP11_1,  // Expected to read a register or device token, but a '{}' token was read instead
                  // - syntax analysis.
        IMP12_1,  // Expected to read a register or identifier token, but a '{}' token was read
                  // instead - syntax analysis.
        IMP13_1,  // Expected to read the device reference token, but ended up reading the '{}'
                  // token
                  // - syntax analysis.
        IMP14_1,  // Expected to read a macro call token, but a '{}' token was read instead - syntax
                  // analysis.
        IMP15_1,  // Expected to read an identifier or a number token, but a '{}' token was read
                  // instead - syntax analysis.
        IMP16_1,  // Expected to read a number token, but a '{}' token was read instead - syntax
                  // analysis.

        // Error

        IEP1_1,  // Unexpected token '{}' - syntax analysis.

        /* Analysis */

        // Error
        IEA1_2,  // '{}' expected, but found '{}' - semantic analysis.
        IEA2_1,  // The symbol '{}' has been redefined - semantic analysis.
        IEA3_1,  // Identifier '{}' has not been defined - semantic analysis.
        IEA4,    // An alias cannot be defined for another alias - semantic analysis.
    };

    template<typename, IC10MsgId>
    struct IC10MsgMap;

    using IC10MsgPack = MsgPack<IC10MsgId, IC10MsgMap>;

    class IC10Local : public Local<IC10MsgPack> {};

    using MsgId = IC10MsgId;

    using Loc = IC10Local;

#define LOCAL_MSG(type, id, _msg)                                                                  \
    template<>                                                                                     \
    struct IC10MsgMap<type, IC10MsgId::id> {                                                       \
        static constexpr auto msg = _msg##_fs;                                                     \
    };

}  // namespace stationeers::ic10


#endif  // COMPILER_LOCAL_HPP
