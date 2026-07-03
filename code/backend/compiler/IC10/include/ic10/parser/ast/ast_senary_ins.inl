// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file ast_senary_ins.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 20:01
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_AST_SENARY_INS_INL
#define COMPILER_AST_SENARY_INS_INL
#pragma once

namespace stationeers::ic10 {

    // SenaryInstructionBase

    template<
        FString K, typename O1, typename O2, typename O3, typename O4, typename O5, typename O6>
    SenaryInstructionBase<K, O1, O2, O3, O4, O5, O6>::SenaryInstructionBase(
        Pos pos, O1 op1, O2 op2, O3 op3, O4 op4, O5 op5, O6 op6
    )
        : QuinaryInstructionBase<K, O1, O2, O3, O4, O5>{pos, op1, op2, op3, op4, op5}
        , operand6(op6)
        , args{op1, op2, op3, op4, op5, op6} {}

    template<
        FString K, typename O1, typename O2, typename O3, typename O4, typename O5, typename O6>
    Pos SenaryInstructionBase<K, O1, O2, O3, O4, O5, O6>::end() const {
        return call(operand6, [](auto&& o) { return o.end(); });
    }

    template<
        FString K, typename O1, typename O2, typename O3, typename O4, typename O5, typename O6>
    std::string SenaryInstructionBase<K, O1, O2, O3, O4, O5, O6>::toString() const {
        if constexpr (IsVariant<O6>)
            return std::format(
                "{} {}", QuinaryInstructionBase<K, O1, O2, O3, O4, O5>::toString(),
                call(operand6, [](auto&& o) { return o.toString(); })
            );
        else
            return std::format(
                "{} {}", QuinaryInstructionBase<K, O1, O2, O3, O4, O5>::toString(),
                operand6.toString()
            );
    }

    template<
        FString K, typename O1, typename O2, typename O3, typename O4, typename O5, typename O6>
    std::string SenaryInstructionBase<K, O1, O2, O3, O4, O5, O6>::toJSON() const {
        return jsonBase();
    }

    template<
        FString K, typename O1, typename O2, typename O3, typename O4, typename O5, typename O6>
    template<typename... Ts>
    std::string SenaryInstructionBase<K, O1, O2, O3, O4, O5, O6>::jsonBase(
        std::pair<std::string, Ts>... fields
    ) const {
        return this
            ->QuinaryInstructionBase<K, O1, O2, O3, O4, O5>::template jsonBase<std::string, Ts...>(
                {"operand6", call(operand6, [](auto&& o) { return o.toJSON(); })}, fields...
            );
    }

}  // namespace stationeers::ic10

#endif  // COMPILER_AST_SENARY_INS_INL
