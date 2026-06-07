// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file ast_quaternary_ins.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 19:59
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_AST_QUATERNARY_INS_INL
#define COMPILER_AST_QUATERNARY_INS_INL
#pragma once

namespace stationeers::ic10 {

    // QuaternaryInstructionBase

    template<FString K, typename O1, typename O2, typename O3, typename O4>
    QuaternaryInstructionBase<K, O1, O2, O3, O4>::QuaternaryInstructionBase(
        Pos pos, O1 op1, O2 op2, O3 op3, O4 op4
    )
        : TernaryInstructionBase<K, O1, O2, O3>{pos, op1, op2, op3}
        , operand4(op4)
        , args{op1, op2, op3, op4} {}

    template<FString K, typename O1, typename O2, typename O3, typename O4>
    Pos QuaternaryInstructionBase<K, O1, O2, O3, O4>::end() const {
        return call(operand4, [](auto&& o) { return o.end(); });
    }

    template<FString K, typename O1, typename O2, typename O3, typename O4>
    std::string QuaternaryInstructionBase<K, O1, O2, O3, O4>::toString() const {
        if constexpr (IsVariant<O4>)
            return std::format(
                "{} {}", TernaryInstructionBase<K, O1, O2, O3>::toString(),
                call(operand4, [](auto&& o) { return o.toString(); })
            );

        else
            return std::format(
                "{} {}", TernaryInstructionBase<K, O1, O2, O3>::operand3.toString(),
                operand4.toString()
            );
    }

    template<FString K, typename O1, typename O2, typename O3, typename O4>
    std::string QuaternaryInstructionBase<K, O1, O2, O3, O4>::toJSON() const {
        return jsonBase();
    }

    template<FString K, typename O1, typename O2, typename O3, typename O4>
    template<typename... Ts>
    std::string QuaternaryInstructionBase<K, O1, O2, O3, O4>::jsonBase(
        std::pair<std::string, Ts>... fields
    ) const {
        return this->TernaryInstructionBase<K, O1, O2, O3>::template jsonBase<std::string, Ts...>(
            {"operand4", call(operand4, [](auto&& o) { return o.toJSON(); })}, fields...
        );
    }


}  // namespace stationeers::ic10

#endif  // COMPILER_AST_QUATERNARY_INS_INL
