// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

/**
 * @file ast_nullary_ins.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 19:52
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_AST_NULLARY_INS_INL
#define COMPILER_AST_NULLARY_INS_INL
#pragma once

namespace stationeers::ic10 {

    // NullaryInstructionBase

    template<FString K>
    NullaryInstructionBase<K>::NullaryInstructionBase(Pos pos)
        : AST<NullaryInstructionBase>{pos} {}

    template<FString K>
    Pos NullaryInstructionBase<K>::end() const {
        return endPos(this->position, keyword.size);
    }

    template<FString K>
    std::string NullaryInstructionBase<K>::toString() const {
        return keyword.value.data();
    }

    template<FString K>
    std::string NullaryInstructionBase<K>::toJSON() const {
        return jsonBase();
    }

    template<FString K>
    template<typename... Ts>
    std::string NullaryInstructionBase<K>::jsonBase(std::pair<std::string, Ts>... fields) const {
        return this->AST<NullaryInstructionBase>::template jsonBase<std::string, Ts...>(
            {"keyword", '"' + std::string(keyword.value.data()) + '"'}, fields...
        );
    }

}  // namespace stationeers::ic10

#endif  // COMPILER_AST_NULLARY_INS_INL
