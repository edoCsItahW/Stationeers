// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file instruction_dispatcher.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/28 16:03
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef STATIONEERS_INSTRUCTION_DISPATCHER_HPP
#define STATIONEERS_INSTRUCTION_DISPATCHER_HPP
#pragma once

#include "ic10_compiler/lexer/token.hpp"
#include "common/utils/enum_to_str.hpp"
#include <variant>


namespace stationeers::ic10 {

    template<InstructionKeyword>
    struct InstructionMapper;

    template<std::size_t N>
    inline constexpr auto to_instruction_keyword = static_cast<InstructionKeyword>(N);

    template<std::size_t N>
    using instruction_map_t = InstructionMapper<to_instruction_keyword<N>>::type;

    template<std::size_t... Is>
    using instruction_t = std::variant<instruction_map_t<Is>...>;

    inline constexpr auto instruction_keyword_len = enumMax<InstructionKeyword>();

    template<typename F>
    auto dispatch(F&& f, InstructionKeyword keyword);

}  // namespace stationeers::ic10

#include "instruction_dispatcher.inl"

#endif  // STATIONEERS_INSTRUCTION_DISPATCHER_HPP
