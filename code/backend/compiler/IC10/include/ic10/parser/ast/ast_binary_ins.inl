// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file ast_binary_ins.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 19:55
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_AST_BINARY_INS_INL
#define COMPILER_AST_BINARY_INS_INL
#pragma once

namespace stationeers::ic10 {

    // BinaryInstructionBase

    template<FString K, OperandType V1, OperandType V2>
    BinaryInstructionBase<K, V1, V2>::BinaryInstructionBase(Pos pos, O1 o1, O2 o2)
        : UnaryInstructionBase<K, V1>{pos, o1}
        , operand2(o2)
        , args(o1, o2) {}

    template<FString K, OperandType V1, OperandType V2>
    Pos BinaryInstructionBase<K, V1, V2>::end() const {
        return call(operand2, [](auto&& o) { return o.end(); });
    }

    template<FString K, OperandType V1, OperandType V2>
    std::string BinaryInstructionBase<K, V1, V2>::toString() const {
        if constexpr (IsVariant<O2>)
            return std::format(
                "{} {}", UnaryInstructionBase<K, V1>::toString(),
                call(operand2, [](auto&& o) { return o.toString(); })
            );

        else
            return std::format(
                "{} {}", UnaryInstructionBase<K, V1>::operand1.toString(), operand2.toString()
            );
    }

    template<FString K, OperandType V1, OperandType V2>
    std::string BinaryInstructionBase<K, V1, V2>::toJSON() const {
        return jsonBase();
    }

    template<FString K, OperandType V1, OperandType V2>
    template<typename... Ts>
    std::string BinaryInstructionBase<K, V1, V2>::jsonBase(
        std::pair<std::string, Ts>... fields
    ) const {
        return this
            ->UnaryInstructionBase<K, V1>::template jsonBase<std::string, std::string_view, Ts...>(
                {"operand2", call(operand2, [](auto&& o) { return o.toJSON(); })},
                {"type2", enumToStr(type2)}, fields...
            );
    }


}  // namespace stationeers::ic10

#endif  // COMPILER_AST_BINARY_INS_INL
