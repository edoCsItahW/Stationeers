// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

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

    template<
        FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4, OperandType V5>
    QuinaryInstructionBase<K, V1, V2, V3, V4, V5>::QuinaryInstructionBase(
        Pos pos, O1 o1, O2 o2, O3 o3, O4 o4, O5 o5
    )
        : QuaternaryInstructionBase<K, V1, V2, V3, V4>{pos, o1, o2, o3, o4}
        , operand5(o5)
        , args(o1, o2, o3, o4, o5) {}

    template<
        FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4, OperandType V5>
    Pos QuinaryInstructionBase<K, V1, V2, V3, V4, V5>::end() const {
        return call(operand5, [](auto&& o) { return o.end(); });
    }

    template<
        FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4, OperandType V5>
    std::string QuinaryInstructionBase<K, V1, V2, V3, V4, V5>::toString() const {
        if constexpr (IsVariant<O5>)
            return std::format(
                "{} {}", QuaternaryInstructionBase<K, V1, V2, V3, V4>::toString(),
                call(operand5, [](auto&& o) { return o.toString(); })
            );
        else
            return std::format(
                "{} {}", QuaternaryInstructionBase<K, V1, V2, V3, V4>::toString(),
                operand5.toString()
            );
    }

    template<
        FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4, OperandType V5>
    std::string QuinaryInstructionBase<K, V1, V2, V3, V4, V5>::toJSON() const {
        return jsonBase();
    }

    template<
        FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4, OperandType V5>
    template<FString... Vs, AstJsonAble... Params>
        requires(sizeof...(Vs) == sizeof...(Params))
    std::string QuinaryInstructionBase<K, V1, V2, V3, V4, V5>::jsonBase(Params&&... params) const {
        return this->QuaternaryInstructionBase<K, V1, V2, V3, V4>::template jsonBase<
            "operand5", "type5", Vs...>(
            operand5, enumToStr(type5), std::forward<Params>(params)...
        );
    }


}  // namespace stationeers::ic10

#endif  // COMPILER_AST_QUINARY_INS_INL
