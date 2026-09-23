// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file parser.inl
 * @author edocsitahw
 * @version 1.1
 * @date 2026/08/28 21:30
 * @if zh
 * @brief @ref Parser 匹配设施的模板实现
 * @details 实现 @ref Parser 的模板匹配接口 @c match、@c matchFirst、@c matchArray、@c matchOperand、@c matchVariant、@c matchPredicate、@c isMatch 与 @c isAnyMatch。
 *          这些接口把"尝试解析某一类 AST 节点"的样板集中到一处：依次比对候选类型的 FIRST 首部序列，命中且通过 @c NodeParser<T>::is 前瞻后才真正调用解析器写入结果，调用方无需自行保存与恢复 Token 游标。
 *
 * @note @c match 按候选类型的逆序尝试（与原型的 @c reversed(types) 一致）；全部候选均不匹配时消费一个 Token、报告 IEP34_1，并产出类型名以 @c | 连接的 @c ErrorNode 而非抛异常，使语法分析得以在错误之后继续。
 * @note @c matchArray 会捕获 @c NodeParser<T>::parse 抛出的 @ref Error 并转换为 @c ErrorNode，因此单个节点的解析失败不会中断整棵语法树的构建。
 *
 * @see parser.hpp @ref Parser 声明
 * @see node_parser.hpp 各节点的解析器
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Template implementation of the @ref Parser matching facilities
 * @details Implements @ref Parser's template matching interface: @c match, @c matchFirst, @c matchArray, @c matchOperand, @c matchVariant, @c matchPredicate, @c isMatch and @c isAnyMatch.
 *          They gather the "try to parse one kind of AST node" boilerplate into one place: candidate types are probed against their FIRST head sequences, and only a hit that also passes the @c NodeParser<T>::is lookahead actually invokes the parser and writes the result, so callers never save and restore the token cursor themselves.
 *
 * @note @c match probes candidate types in reverse order (matching the @c reversed(types) prototype); when nothing matches it consumes one token, reports IEP34_1 and yields an @c ErrorNode whose type names are joined with @c | instead of throwing, so parsing continues past the error.
 * @note @c matchArray catches the @ref Error thrown by @c NodeParser<T>::parse and converts it into an @c ErrorNode, so one failing node does not abort construction of the whole syntax tree.
 *
 * @see parser.hpp @ref Parser declaration
 * @see node_parser.hpp per-node parsers
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef IC10_COMPILER_CORE_PARSER_INL
#define IC10_COMPILER_CORE_PARSER_INL
#pragma once

#include "common/exception/debug.hpp"

namespace stationeers::ic10 {

    namespace detail {

        template<HasFirst Node>
        using first_tuple_t = std::remove_cvref_t<decltype(Node::FIRST)>;

        template<HasFirst Node, std::size_t I>
        inline constexpr auto get_array = std::get<I>(Node::FIRST);

    }  // namespace detail

    template<OperandType O>
    auto Parser::matchOperand() {
        // 通过operand_type_t将OperandType映射为语义变体
        return matchVariant<operand_type_t<O>>();
    }

    template<IsVariant Variant>
    auto Parser::matchVariant() {
        return [this]<HasFirst... Ts>(std::variant<Ts...>*) {  // 萃取
            return match<Ts...>();
        }(static_cast<Variant*>(nullptr));
    }

    /**
     *
     * 原型:
     * @code{python}
     * for type in reversed(types):
     *     for seq in type.FIRST:
     *         if tokens[start:start + len(seq)] == seq:
     *             if ntype.was is None or type.was(parser):
     *                 parser.idx += len(seq)
     *                 node = type(None)
     *                 print(f"Parsed: {node}")
     *                 return node
     *             break
     * @endcode
     *
     * @tparam Ts 节点类型
     * @return
     */
    template<HasFirst... Ts>
    auto Parser::match() {
        constexpr std::size_t N = sizeof...(Ts);

        Errorable<Ts...> result;

        // for type in reversed(types):
        auto match = [&, this]<std::size_t... Is>(std::index_sequence<Is...>) {
            return (
                ... || (matchFirst<std::tuple_element_t<N - 1 - Is, std::tuple<Ts...>>>(result))
            );
        }(std::make_index_sequence<N>{});

        if (!match) [[unlikely]] {
            const auto& tokenPtr = current();

            consume();

            std::string types{};
            bool first = true;

            ((types += (first ? "" : "|") + std::string(Ts::nodeName), first = false), ...);

            reporter_.errorWith<ICMsgId::IEP34_1>(tokenPtr->pos, endPos(*tokenPtr), types);
            result = ErrorNode{*tokenPtr, ICLoc::msgFormat<ICMsgId::IEP34_1>(types)};
        }

        return result;
    }

    template<HasFirst T>
    bool Parser::matchFirst(auto& result) noexcept {
        // for seq in type.FIRST:
        return [&, this]<std::size_t... Is>(std::index_sequence<Is...>) {
            return (
                ...
                || (idx_ + detail::get_array<T, Is>.size() < tokens_.size()
                    && matchArray<T, detail::get_array<T, Is>>(result))
            );
        }(std::make_index_sequence<std::tuple_size_v<detail::first_tuple_t<T>>>{});
    }

    template<HasFirst T, std::array Array>
    bool Parser::matchArray(auto& result) noexcept {
        using R = std::remove_reference_t<decltype(result)>;

        // if tokens[start:start + len(seq)] == seq:
        if ([this]<std::size_t... Is>(std::index_sequence<Is...>) {
                return (... && (Array[Is] == peek(Is)->type));
            }(std::make_index_sequence<Array.size()>{})) {
            if (matchPredicate<T>()) {
                try {
                    if constexpr (IsVariant<decltype(NodeParser<T>::parse(*this))>)  // 结果是变体
                        result = wide_cast<R>(NodeParser<T>::parse(*this));
                    else
                        result = NodeParser<T>::parse(*this);

                } catch (const Error& e) {
                    if (auto tokenPtr = current(); tokenPtr)
                        result = ErrorNode{*tokenPtr, e.message().data()};
                    else
                        result = ErrorNode{Token{}, e.message().data()};
                }
                return true;
            }
            return false;
        }

        return false;
    }

    template<HasFirst T>
    bool Parser::matchPredicate() noexcept {
        // if ntype.was is None or type.was(parser):
        if constexpr (requires {
                          { NodeParser<T>::is(*this) } -> std::same_as<bool>;
                      })
            return NodeParser<T>::is(*this);

        return true;
    }

    template<HasFirst T>
    bool Parser::isMatch() noexcept {
        return [this]<std::size_t... Is>(std::index_sequence<Is...>) {
            return (
                ...
                || (idx_ + detail::get_array<T, Is>.size() < tokens_.size()
                    && [this]<std::size_t... Js>(std::index_sequence<Js...>) {
                        constexpr auto array = detail::get_array<T, Is>;
                        return (... && (array[Js] == peek(Js)->type));
                    }(std::make_index_sequence<detail::get_array<T, Is>.size()>{}))
            );
        }(std::make_index_sequence<std::tuple_size_v<detail::first_tuple_t<T>>>{});
    }

    template<HasFirst... Ts>
    bool Parser::isAnyMatch() noexcept {
        using Tuple             = std::tuple<Ts...>;
        constexpr std::size_t N = sizeof...(Ts);

        return [this]<std::size_t... Is>(std::index_sequence<Is...>) {
            return (... || (isMatch<std::tuple_element_t<N - 1 - Is, Tuple>>()));
        }(std::make_index_sequence<N>{});
    }

    template<IsVariant Variant>
    bool Parser::isVariantMatch() noexcept {
        return [this]<HasFirst... Ts>(std::variant<Ts...>*) {  // 萃取
            return isAnyMatch<Ts...>();
        }(static_cast<Variant*>(nullptr));
    }

}  // namespace stationeers::ic10

#include "expand_node_parser.hpp"

#endif  // IC10_COMPILER_CORE_PARSER_INL
