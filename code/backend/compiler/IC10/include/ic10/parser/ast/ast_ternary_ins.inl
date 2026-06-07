// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file ast_ternary_ins.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 19:57
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_AST_TERNARY_INS_INL
#define COMPILER_AST_TERNARY_INS_INL
#pragma once

namespace stationeers::ic10 {

    // TernaryInstructionBase

    template<FString K, typename O1, typename O2, typename O3>
    TernaryInstructionBase<K, O1, O2, O3>::TernaryInstructionBase(Pos pos, O1 op1, O2 op2, O3 op3)
        : BinaryInstructionBase<K, O1, O2>{pos, op1, op2}
        , operand3(op3)
        , args{op1, op2, op3} {}

    template<FString K, typename O1, typename O2, typename O3>
    Pos TernaryInstructionBase<K, O1, O2, O3>::end() const {
        return call(operand3, [](auto&& o) { return o.end(); });
    }

    template<FString K, typename O1, typename O2, typename O3>
    std::string TernaryInstructionBase<K, O1, O2, O3>::toString() const {
        if constexpr (IsVariant<O3>)
            return std::format(
                "{} {}", BinaryInstructionBase<K, O1, O2>::toString(),
                call(operand3, [](auto&& o) { return o.toString(); })
            );

        else
            return std::format(
                "{} {}", BinaryInstructionBase<K, O1, O2>::operand2.toString(), operand3.toString()
            );
    }

    template<FString K, typename O1, typename O2, typename O3>
    std::string TernaryInstructionBase<K, O1, O2, O3>::toJSON() const {
        return jsonBase();
    }

    template<FString K, typename O1, typename O2, typename O3>
    template<typename... Ts>
    std::string TernaryInstructionBase<K, O1, O2, O3>::jsonBase(
        std::pair<std::string, Ts>... fields
    ) const {
        return this->BinaryInstructionBase<K, O1, O2>::template jsonBase<std::string, Ts...>(
            {"operand3", call(operand3, [](auto&& o) { return o.toJSON(); })}, fields...
        );
    }

}  // namespace stationeers::ic10

#endif  // COMPILER_AST_TERNARY_INS_INL
