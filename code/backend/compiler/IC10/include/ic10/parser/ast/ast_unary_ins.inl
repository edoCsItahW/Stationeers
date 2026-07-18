// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file ast_unary_ins.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 19:53
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_AST_UNARY_INS_INL
#define COMPILER_AST_UNARY_INS_INL
#pragma once

#include "common/utils/common.hpp"
#include <format>

namespace stationeers::ic10 {

    // UnaryInstructionBase

    template<FString K, OperandType V1>
    UnaryInstructionBase<K, V1>::UnaryInstructionBase(Pos pos, O1 o1)
        : NullaryInstructionBase<K>{pos}
        , operand1(o1)
        , args{o1} {}

    template<FString K, OperandType V1>
    Pos UnaryInstructionBase<K, V1>::end() const {
        return call(operand1, [](auto&& o) { return o.end(); });
    }

    template<FString K, OperandType V1>
    std::string UnaryInstructionBase<K, V1>::toString() const {
        if constexpr (IsVariant<O1>)
            return std::format(
                "{} {}", NullaryInstructionBase<K>::toString(),
                call(operand1, [](auto&& o) { return o.toString(); })
            );

        else
            return std::format("{} {}", NullaryInstructionBase<K>::toString(), operand1.toString());
    }

    template<FString K, OperandType V1>
    std::string UnaryInstructionBase<K, V1>::toJSON() const {
        return jsonBase();
    }

    template<FString K, OperandType V1>
    template<typename... Ts>
    std::string UnaryInstructionBase<K, V1>::jsonBase(std::pair<std::string, Ts>... fields) const {
        return this
            ->NullaryInstructionBase<K>::template jsonBase<std::string, std::string_view, Ts...>(
                {"operand1", call(operand1, [](auto&& o) { return o.toJSON(); })},
                {"type1", enumToStr(type1)}, fields...
            );
    }

}  // namespace stationeers::ic10

#endif  // COMPILER_AST_UNARY_INS_INL
