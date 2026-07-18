// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

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

    template<FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4>
    QuaternaryInstructionBase<K, V1, V2, V3, V4>::QuaternaryInstructionBase(
        Pos pos, O1 o1, O2 o2, O3 o3, O4 o4
    )
        : TernaryInstructionBase<K, V1, V2, V3>{pos, o1, o2, o3}
        , operand4(o4)
        , args(o1, o2, o3, o4) {}

    template<FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4>
    Pos QuaternaryInstructionBase<K, V1, V2, V3, V4>::end() const {
        return call(operand4, [](auto&& o) { return o.end(); });
    }

    template<FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4>
    std::string QuaternaryInstructionBase<K, V1, V2, V3, V4>::toString() const {
        if constexpr (IsVariant<O4>)
            return std::format(
                "{} {}", TernaryInstructionBase<K, V1, V2, V3>::toString(),
                call(operand4, [](auto&& o) { return o.toString(); })
            );

        else
            return std::format(
                "{} {}", TernaryInstructionBase<K, V1, V2, V3>::operand3.toString(),
                operand4.toString()
            );
    }

    template<FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4>
    std::string QuaternaryInstructionBase<K, V1, V2, V3, V4>::toJSON() const {
        return jsonBase();
    }

    template<FString K, OperandType V1, OperandType V2, OperandType V3, OperandType V4>
    template<typename... Ts>
    std::string QuaternaryInstructionBase<K, V1, V2, V3, V4>::jsonBase(
        std::pair<std::string, Ts>... fields
    ) const {
        return this->TernaryInstructionBase<K, V1, V2, V3>::template jsonBase<
            std::string, std::string_view, Ts...>(
            {"operand4", call(operand4, [](auto&& o) { return o.toJSON(); })},
            {"type4", enumToStr(type4)}, fields...
        );
    }


}  // namespace stationeers::ic10

#endif  // COMPILER_AST_QUATERNARY_INS_INL
