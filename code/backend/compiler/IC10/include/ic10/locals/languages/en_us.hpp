// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file en_us.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/02 23:49
 * @brief
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

#define LOCAL_MSG_EN_US(id, msg) LOCAL_MSG(EnUs, id, msg)

    LOCAL_MSG_EN_US(
        IWL1,
        "The END token was not encountered as expected at the end of the file - lexical analysis."
    )

    LOCAL_MSG_EN_US(IEL1_1, "Unknown operation symbol'{}' - lexical analysis.")

    LOCAL_MSG_EN_US(IMP1, "Unexpectedly encountered the end of the file - syntax analysis.")

    LOCAL_MSG_EN_US(
        IMP2_1,
        "Expected to read a preprocessing directive token, but a '{}' token was read instead "
        "- syntax analysis."
    )

    LOCAL_MSG_EN_US(
        IMP3_1,
        "Expected to read a executable instruction token, but a '{}' token was read instead "
        "- syntax analysis."
    )

    LOCAL_MSG_EN_US(
        IMP4_1, "Expected to read a unary instruction token, but a '{}' token was read instead - "
                "syntax analysis."
    )

    LOCAL_MSG_EN_US(
        IMP5_1, "Expected to read a binary instruction token, but a '{}' token was read instead - "
                "syntax analysis."
    )

    LOCAL_MSG_EN_US(
        IMP6_1, "Expected to read a ternary instruction token, but a '{}' token was read instead - "
                "syntax analysis."
    )

    LOCAL_MSG_EN_US(
        IMP7_1,
        "Expected to read a quaternary instruction token, but a '{}' token was read instead "
        "- syntax analysis."
    )

    LOCAL_MSG_EN_US(
        IMP8_1, "Expected to read a quinary instruction token, but a '{}' token was read instead - "
                "syntax analysis."
    )

    LOCAL_MSG_EN_US(
        IMP9_1, "Expected to read a senary instruction token, but a '{}' token was read instead - "
                "syntax analysis."
    )

    LOCAL_MSG_EN_US(
        IMP10_1,
        "Expected to read a operand tokens, but a '{}' token was read instead - syntax analysis."
    )

    LOCAL_MSG_EN_US(
        IMP11_1, "Expected to read a register or device token, but a '{}' token was read instead - "
                 "syntax analysis."
    )

    LOCAL_MSG_EN_US(
        IMP12_1,
        "Expected to read a register or identifier token, but a '{}' token was read instead "
        "- syntax analysis."
    )

    LOCAL_MSG_EN_US(
        IMP13_1,
        "Expected to read the device reference token, but ended up reading the '{}' token - "
        "syntax analysis."
    )

    LOCAL_MSG_EN_US(
        IMP14_1,
        "Expected to read a macro call token, but a '{}' token was read instead - syntax analysis."
    )

    LOCAL_MSG_EN_US(
        IMP15_1, "Expected to read an identifier or a number token, but a '{}' token was read "
                 "instead - syntax analysis."
    )

    LOCAL_MSG_EN_US(
        IMP16_1,
        "Expected to read a number token, but a '{}' token was read instead - syntax analysis."
    )

    LOCAL_MSG_EN_US(IEP1_1, "Unexpected token '{}' - syntax analysis.")

    LOCAL_MSG_EN_US(IEA1_2, "'{}' expected, but found '{}' - syntax analysis.")

    LOCAL_MSG_EN_US(IEA2_1, "The symbol '{}' has been redefined - semantic analysis.")

    LOCAL_MSG_EN_US(IEA3_1, "Identifier '{}' has not been defined - semantic analysis.")

    LOCAL_MSG_EN_US(IEA4, "An alias cannot be defined for another alias - semantic analysis.")

}  // namespace stationeers::ic10

#endif  // COMPILER_EN_US_HPP
