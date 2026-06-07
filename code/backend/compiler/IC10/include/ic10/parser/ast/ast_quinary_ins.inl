// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file ast_quinary_ins.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 20:00
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_AST_QUINARY_INS_INL
#define COMPILER_AST_QUINARY_INS_INL
#pragma once

namespace stationeers::ic10 {

    // QuinaryInstructionBase

    template<FString K, typename O1, typename O2, typename O3, typename O4, typename O5>
    QuinaryInstructionBase<K, O1, O2, O3, O4, O5>::QuinaryInstructionBase(
        Pos pos, O1 op1, O2 op2, O3 op3, O4 op4, O5 op5
    )
        : QuaternaryInstructionBase<K, O1, O2, O3, O4>{pos, op1, op2, op3, op4}
        , operand5(op5)
        , args{op1, op2, op3, op4, op5} {}

    template<FString K, typename O1, typename O2, typename O3, typename O4, typename O5>
    Pos QuinaryInstructionBase<K, O1, O2, O3, O4, O5>::end() const {
        return call(operand5, [](auto&& o) { return o.end(); });
    }

    template<FString K, typename O1, typename O2, typename O3, typename O4, typename O5>
    std::string QuinaryInstructionBase<K, O1, O2, O3, O4, O5>::toString() const {
        if constexpr (IsVariant<O5>)
            return std::format(
                "{} {}", QuaternaryInstructionBase<K, O1, O2, O3, O4>::toString(),
                call(operand5, [](auto&& o) { return o.toString(); })
            );
        else
            return std::format(
                "{} {}", QuaternaryInstructionBase<K, O1, O2, O3, O4>::toString(),
                operand5.toString()
            );
    }

    template<FString K, typename O1, typename O2, typename O3, typename O4, typename O5>
    std::string QuinaryInstructionBase<K, O1, O2, O3, O4, O5>::toJSON() const {
        return jsonBase();
    }

    template<FString K, typename O1, typename O2, typename O3, typename O4, typename O5>
    template<typename... Ts>
    std::string QuinaryInstructionBase<K, O1, O2, O3, O4, O5>::jsonBase(
        std::pair<std::string, Ts>... fields
    ) const {
        return this
            ->QuaternaryInstructionBase<K, O1, O2, O3, O4>::template jsonBase<std::string, Ts...>(
                {"operand5", call(operand5, [](auto&& o) { return o.toJSON(); })}, fields...
            );
    }


}  // namespace stationeers::ic10

#endif  // COMPILER_AST_QUINARY_INS_INL
