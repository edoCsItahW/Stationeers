// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

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

    template<FString K, OperandType V1, OperandType V2, OperandType V3>
    TernaryInstructionBase<K, V1, V2, V3>::TernaryInstructionBase(Pos pos, O1 o1, O2 o2, O3 o3)
        : BinaryInstructionBase<K, V1, V2>{pos, o1, o2}
        , operand3(o3)
        , args(o1, o2, o3) {}

    template<FString K, OperandType V1, OperandType V2, OperandType V3>
    Pos TernaryInstructionBase<K, V1, V2, V3>::end() const {
        return call(operand3, [](auto&& o) { return o.end(); });
    }

    template<FString K, OperandType V1, OperandType V2, OperandType V3>
    std::string TernaryInstructionBase<K, V1, V2, V3>::toString() const {
        if constexpr (IsVariant<O3>)
            return std::format(
                "{} {}", BinaryInstructionBase<K, V1, V2>::toString(),
                call(operand3, [](auto&& o) { return o.toString(); })
            );

        else
            return std::format(
                "{} {}", BinaryInstructionBase<K, V1, V2>::operand2.toString(), operand3.toString()
            );
    }

    template<FString K, OperandType V1, OperandType V2, OperandType V3>
    std::string TernaryInstructionBase<K, V1, V2, V3>::toJSON() const {
        return jsonBase();
    }

    template<FString K, OperandType V1, OperandType V2, OperandType V3>
    template<typename... Ts>
    std::string TernaryInstructionBase<K, V1, V2, V3>::jsonBase(
        std::pair<std::string, Ts>... fields
    ) const {
        return this->BinaryInstructionBase<K, V1, V2>::template jsonBase<
            std::string, std::string_view, Ts...>(
            {"operand3", call(operand3, [](auto&& o) { return o.toJSON(); })},
            {"type3", enumToStr(type3)}, fields...
        );
    }

}  // namespace stationeers::ic10

#endif  // COMPILER_AST_TERNARY_INS_INL
