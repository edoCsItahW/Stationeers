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
#ifndef IC10_COMPILER_CORE_INSTRUCTION_DISPATCHER_HPP
#define IC10_COMPILER_CORE_INSTRUCTION_DISPATCHER_HPP
#pragma once

#include "ic10_compiler/lexer/token.hpp"
#include "common/utils/enum_to_str.hpp"
#include <optional>
#include <variant>


namespace stationeers::ic10 {

    template<InstructionKeyword>
    struct InstructionMapper;

    template<std::size_t N>
    inline constexpr auto to_instruction_keyword = static_cast<InstructionKeyword>(N);

    template<std::size_t N>
    using instruction_map_t = InstructionMapper<to_instruction_keyword<N>>::type;

    inline constexpr auto instruction_keyword_len = enumMax<InstructionKeyword>();

    /**
     * @if zh
     *
     * @brief 按运行时指令关键字分派到对应的指令节点
     * @details 编译期展开全部 @ref InstructionKeyword 取值，运行时只进入关键字命中的那一支：
     *          调用 @p f 解析该指令的操作数并构造节点，再以 @c std::in_place_type 直接就地
     *          构造 @p Result 中的对应分支。全部取值均不匹配时返回空 @c std::optional。
     *
     * @tparam Result 目标变体类型（如 @c ExecutableInstruction）
     * @tparam F 指令回调类型，签名需支持任意 @c Ins<K, Vs...>
     * @param f 指令回调
     * @param keyword 运行时关键字
     * @return 命中时返回构造好的 @p Result，否则返回空 @c std::optional
     *
     * @note 分派目标由调用方以 @p Result 指定，避免在分派处先构造一个覆盖全部指令的中间变体、
     *       再整体转换一次：那样每个关键字分支都要对 N 个候选分支做一次可构造性检查。
     * @see InstructionMapper 关键字到指令节点的映射
     *
     * @elseif en
     *
     * @brief Dispatch to the instruction node selected by a runtime keyword
     * @details Every @ref InstructionKeyword value is expanded at compile time and at runtime only
     *          the branch matching the keyword is entered: @p f parses that instruction's operands
     *          and builds the node, which is then constructed directly into the matching branch of
     *          @p Result via @c std::in_place_type. Returns an empty @c std::optional when no value
     *          matches.
     *
     * @tparam Result Target variant type (e.g. @c ExecutableInstruction)
     * @tparam F Instruction callback type, invocable with any @c Ins<K, Vs...>
     * @param f Instruction callback
     * @param keyword Runtime keyword
     * @return The built @p Result when matched, otherwise an empty @c std::optional
     *
     * @note The dispatch target is supplied by the caller as @p Result, so no intermediate variant
     *       covering all instructions is built and converted afterwards: that would force a
     *       constructibility check against N alternatives inside every keyword branch.
     * @see InstructionMapper keyword to instruction node mapping
     *
     * @endif
     */
    template<typename Result, typename F>
    std::optional<Result> dispatch(F&& f, InstructionKeyword keyword);

}  // namespace stationeers::ic10

#include "instruction_dispatcher.inl"

#endif  // IC10_COMPILER_CORE_INSTRUCTION_DISPATCHER_HPP
