// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file ast_node.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/06 22:32
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef COMPILER_AST_NODE_INL
#define COMPILER_AST_NODE_INL
#pragma once

#include "common/utils/common.hpp"
#include <algorithm>
#include <format>
#include <ranges>
#include <sstream>

namespace stationeers::ic10 {

    // AST

    template<typename Derived>
    AST<Derived>::AST(Pos pos)
        : position{pos} {}

    template<typename Derived>
    Pos AST<Derived>::start() const {
        return position;
    }

    template<typename Derived>
    template<typename T>
    std::string AST<Derived>::process(T&& arg) {
        using U = std::decay_t<T>;

        if constexpr (IsVariant<U>)
            return call(arg, [](auto&& o) { return process(o); });

        else if constexpr (JsonStringAble<U>)
            return toJsonString(arg);

        else if constexpr (requires { arg.toJSON(); })
            return arg.toJSON();

        else
            return arg;
    }

    template<typename Derived>
    template<typename T, typename F>
    std::string AST<Derived>::seqJSON(const std::vector<T>& datas, F func) {
        std::stringstream ss;

        ss << "[";

        for (std::size_t i = 0; i < datas.size(); ++i)
            ss << func(datas[i]) << (i == datas.size() - 1 ? "" : ",");

        ss << "]";

        return ss.str();
    }

    template<typename Derived>
    template<FString... Vs, AstJsonAble... Args>
        requires(sizeof...(Vs) == sizeof...(Args))
    std::string AST<Derived>::jsonBase(Args&&... args) const {
        return toJson<"type", "position", Vs...>(
            Derived::nodeName, toJson<"line", "column">(position.line(), position.column()),
            [](auto&& arg) -> std::optional<std::string> {
                using U = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<U, std::nullopt_t>)
                    return std::nullopt;
                else if constexpr (IsOptional<U>)
                    if (!arg.has_value())
                        return std::nullopt;
                    else
                        return process(*arg);
                else
                    return process(arg);
            }(std::forward<Args>(args))...
        );
    }

    namespace detail {

        // 编译期计算多个 std::array 的总大小（fold expression，非递归以兼容 MSVC）
        template<typename... Arrays>
        constexpr std::size_t totalArraySize() {
            return (0 + ... + Arrays{}.size());
        }

        // 编译期合并多个 array 到固定大小的结果数组（单层拷贝，非递归）
        template<std::size_t N, typename... Arrays>
        constexpr std::array<TokenType, N> concatArraysDirect(Arrays... arrays) {
            std::array<TokenType, N> result{};
            std::size_t idx = 0;
            ((void)(std::copy(arrays.begin(), arrays.end(), result.begin() + idx),
                    idx += arrays.size()),
             ...);
            return result;
        }

        // 编译期数组合并的便捷封装：自动推导结果大小
        template<typename... Arrays>
        constexpr auto concatArrays(Arrays... arrays) {
            constexpr std::size_t N = totalArraySize<Arrays...>();
            return concatArraysDirect<N>(arrays...);
        }

        // variant token 集合推导：通过索引序列展开每个 alternative 并合并
        template<IsVariant Variant, std::size_t... Is>
        constexpr auto variantTokensImpl(std::index_sequence<Is...>) {
            return concatArrays(extractTokens<std::variant_alternative_t<Is, Variant>>()...);
        }

        // 推导 variant 类型所有成员的起始 token 集合（含嵌套 variant 展开）
        template<IsVariant Variant>
        constexpr auto variantTokens() {
            if constexpr (IsVariant<Variant>)
                return variantTokensImpl<Variant>(
                    std::make_index_sequence<std::variant_size_v<Variant>>()
                );

            else
                return extractTokens<Variant>();
        }

        // 提取类型 T 的起始 token 集合
        // - variant 类型：递归展开所有 alternative
        // - 含 startTokens 的类型：直接返回
        // - 其他类型：返回空数组
        template<typename T>
        constexpr auto extractTokens() {
            if constexpr (IsVariant<T>)
                return variantTokens<T>();

            else if constexpr (HasStartTokens<T>)
                return T::startTokens;

            else
                return std::array<TokenType, 0>{};
        }

    }  // namespace detail

    // 获取 variant 类型所有成员的起始 token 集合（编译期推导）
    template<IsVariant Variant>
    constexpr auto getStartTokens() {
        return detail::extractTokens<Variant>();
    }

    // 判断给定 token 类型是否为 variant 的合法起始
    template<IsVariant Variant>
    constexpr bool isStartToken(TokenType t) {
        constexpr auto tokens = getStartTokens<Variant>();
        for (auto token : tokens)
            if (token == t) return true;

        return false;
    }

    // 判断 token 是否为合法操作数起始（Operand variant 的便捷封装）
    constexpr bool isOperandStart(TokenType t) { return isStartToken<Operand>(t); }

    constexpr bool isStatementStart(TokenType t) {
        using T = TokenType;
        switch (t) {
            // 标签（identifier 后跟冒号）
            case T::IDENTIFIER:

            // 预处理指令
            case T::KEYWORD_ALIAS:
            case T::KEYWORD_DEFINE:

            // 零元指令
            case T::KEYWORD_HCF:
            case T::KEYWORD_YIELD:

            // 一元指令
            case T::KEYWORD_PEEK:
            case T::KEYWORD_POP:
            case T::KEYWORD_PUSH:
            case T::KEYWORD_CLR:
            case T::KEYWORD_J:
            case T::KEYWORD_JAL:
            case T::KEYWORD_JR:
            case T::KEYWORD_RAND:
            case T::KEYWORD_SLEEP:
            case T::KEYWORD_CLRD:

            // 二元指令
            case T::KEYWORD_ABS:
            case T::KEYWORD_ACOS:
            case T::KEYWORD_ADD:
            case T::KEYWORD_ASIN:
            case T::KEYWORD_ATAN:
            case T::KEYWORD_ATAN2:
            case T::KEYWORD_CEIL:
            case T::KEYWORD_COS:
            case T::KEYWORD_DIV:
            case T::KEYWORD_EXP:
            case T::KEYWORD_FLOOR:
            case T::KEYWORD_LOG:
            case T::KEYWORD_MAX:
            case T::KEYWORD_MIN:
            case T::KEYWORD_MOD:
            case T::KEYWORD_MUL:
            case T::KEYWORD_POW:
            case T::KEYWORD_ROUND:
            case T::KEYWORD_SIN:
            case T::KEYWORD_SQRT:
            case T::KEYWORD_SGN:
            case T::KEYWORD_SUB:
            case T::KEYWORD_TAN:
            case T::KEYWORD_TRUNC:
            case T::KEYWORD_NOT:
            case T::KEYWORD_MOVE:
            case T::KEYWORD_POKE:
            case T::KEYWORD_BEQZ:
            case T::KEYWORD_BEQZAL:
            case T::KEYWORD_BNEZ:
            case T::KEYWORD_BNEZAL:
            case T::KEYWORD_BGEZ:
            case T::KEYWORD_BGEZAL:
            case T::KEYWORD_BGTZ:
            case T::KEYWORD_BGTZAL:
            case T::KEYWORD_BLEZ:
            case T::KEYWORD_BLEZAL:
            case T::KEYWORD_BLTZ:
            case T::KEYWORD_BLTZAL:
            case T::KEYWORD_BNAN:
            case T::KEYWORD_BDNS:
            case T::KEYWORD_BDNSAL:
            case T::KEYWORD_BDSE:
            case T::KEYWORD_BDSEAL:
            case T::KEYWORD_BREQZ:
            case T::KEYWORD_BRNEZ:
            case T::KEYWORD_BRGEZ:
            case T::KEYWORD_BRGTZ:
            case T::KEYWORD_BRLEZ:
            case T::KEYWORD_BRLTZ:
            case T::KEYWORD_BRNAN:
            case T::KEYWORD_BRDNS:
            case T::KEYWORD_BRDSE:
            case T::KEYWORD_SEQZ:
            case T::KEYWORD_SNEZ:
            case T::KEYWORD_SGEZ:
            case T::KEYWORD_SGTZ:
            case T::KEYWORD_SLEZ:
            case T::KEYWORD_SLTZ:
            case T::KEYWORD_SNAN:
            case T::KEYWORD_SNANZ:
            case T::KEYWORD_SDNS:
            case T::KEYWORD_SDSE:

            // 三元指令
            case T::KEYWORD_AND:
            case T::KEYWORD_NOR:
            case T::KEYWORD_OR:
            case T::KEYWORD_SLA:
            case T::KEYWORD_SLL:
            case T::KEYWORD_SRA:
            case T::KEYWORD_SRL:
            case T::KEYWORD_XOR:
            case T::KEYWORD_GET:
            case T::KEYWORD_PUT:
            case T::KEYWORD_L:
            case T::KEYWORD_LS:
            case T::KEYWORD_S:
            case T::KEYWORD_SB:
            case T::KEYWORD_RMAP:
            case T::KEYWORD_ROL:
            case T::KEYWORD_ROR:
            case T::KEYWORD_BEQ:
            case T::KEYWORD_BEQAL:
            case T::KEYWORD_BNE:
            case T::KEYWORD_BNEAL:
            case T::KEYWORD_BGE:
            case T::KEYWORD_BGEAL:
            case T::KEYWORD_BGT:
            case T::KEYWORD_BGTAL:
            case T::KEYWORD_BLE:
            case T::KEYWORD_BLEAL:
            case T::KEYWORD_BLT:
            case T::KEYWORD_BLTAL:
            case T::KEYWORD_BAPZ:
            case T::KEYWORD_BAPZAL:
            case T::KEYWORD_BNAZ:
            case T::KEYWORD_BNAZAL:
            case T::KEYWORD_BDNVL:
            case T::KEYWORD_BDNVS:
            case T::KEYWORD_BREQ:
            case T::KEYWORD_BRNE:
            case T::KEYWORD_BRGE:
            case T::KEYWORD_BRGT:
            case T::KEYWORD_BRLE:
            case T::KEYWORD_BRLT:
            case T::KEYWORD_BRAPZ:
            case T::KEYWORD_BRNAZ:
            case T::KEYWORD_SAPZ:
            case T::KEYWORD_SNAZ:
            case T::KEYWORD_SEQ:
            case T::KEYWORD_SNE:
            case T::KEYWORD_SGE:
            case T::KEYWORD_SGT:
            case T::KEYWORD_SLE:
            case T::KEYWORD_SLT:

            // 四元指令
            case T::KEYWORD_LERP:
            case T::KEYWORD_CLAMP:
            case T::KEYWORD_LR:
            case T::KEYWORD_EXT:
            case T::KEYWORD_INS:
            case T::KEYWORD_SS:
            case T::KEYWORD_LB:
            case T::KEYWORD_SBN:
            case T::KEYWORD_SBS:
            case T::KEYWORD_BAP:
            case T::KEYWORD_BAPAL:
            case T::KEYWORD_BNA:
            case T::KEYWORD_BNAAL:
            case T::KEYWORD_BRAP:
            case T::KEYWORD_BRNA:
            case T::KEYWORD_SAP:
            case T::KEYWORD_SNA:
            case T::KEYWORD_SELECT:

            // 五元指令
            case T::KEYWORD_LBN:
            case T::KEYWORD_LBS:

            // 六元指令
            case T::KEYWORD_LBNS: return true;

            default: return false;
        }
    }

}  // namespace stationeers::ic10

#endif  // COMPILER_AST_NODE_INL
