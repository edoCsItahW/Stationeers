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
        FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4, OperandType V5,
        OperandType V6>
    SenaryInstructionBase<K, V1, V2, V3, V4, V5, V6>::SenaryInstructionBase(
        Pos pos, O1 o1, O2 o2, O3 o3, O4 o4, O5 o5, O6 o6
    )
        : QuinaryInstructionBase<K, V1, V2, V3, V4, V5>{pos, o1, o2, o3, o4, o5}
        , operand6(o6)
        , args(o1, o2, o3, o4, o5, o6) {}

    template<
        FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4, OperandType V5,
        OperandType V6>
    Pos SenaryInstructionBase<K, V1, V2, V3, V4, V5, V6>::end() const {
        return call(operand6, [](auto&& o) { return o.end(); });
    }

    template<
        FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4, OperandType V5,
        OperandType V6>
    std::string SenaryInstructionBase<K, V1, V2, V3, V4, V5, V6>::toString() const {
        if constexpr (IsVariant<O6>)
            return std::format(
                "{} {}", QuinaryInstructionBase<K, V1, V2, V3, V4, V5>::toString(),
                call(operand6, [](auto&& o) { return o.toString(); })
            );
        else
            return std::format(
                "{} {}", QuinaryInstructionBase<K, V1, V2, V3, V4, V5>::toString(),
                operand6.toString()
            );
    }

    template<
        FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4, OperandType V5,
        OperandType V6>
    std::string SenaryInstructionBase<K, V1, V2, V3, V4, V5, V6>::toJSON() const {
        return jsonBase();
    }

    template<
        FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4, OperandType V5,
        OperandType V6>
    template<FString... Vs, AstJsonAble... Params>
        requires(sizeof...(Vs) == sizeof...(Params))
    std::string SenaryInstructionBase<K, V1, V2, V3, V4, V5, V6>::jsonBase(
        Params&&... params
    ) const {
        return this->QuinaryInstructionBase<K, V1, V2, V3, V4, V5>::template jsonBase<
            "operand6", "type6", Vs...>(
            operand6, static_cast<int>(type6), std::forward<Params>(params)...
        );
    }

}  // namespace stationeers::ic10

#endif  // COMPILER_AST_SENARY_INS_INL
