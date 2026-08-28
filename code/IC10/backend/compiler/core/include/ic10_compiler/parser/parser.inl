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
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#ifndef STATIONEERS_PARSER_INL
#define STATIONEERS_PARSER_INL
#pragma once

#include "common/exception/debug.hpp"
#include "expand_node_parser.hpp"

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
            result = ErrorNode{*tokenPtr, std::move(types)};
        }

        return result;
    }

    template<HasFirst T>
    bool Parser::matchFirst(auto& result) noexcept {
        // for seq in type.FIRST:
        return [&, this]<std::size_t... Is>(std::index_sequence<Is...>) {
            return (... || (matchArray<T, detail::get_array<T, Is>>(result)));
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
                    if constexpr ( IsVariant<decltype(NodeParser<T>::parse(*this))> )  // 结果是变体
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
            return (... || ([this]<std::size_t... Js>(std::index_sequence<Js...>) {
                        constexpr auto array = detail::get_array<T, Is>;
                        return (... && (array[Js] == peek(Js)->type));
                    }(std::make_index_sequence<detail::get_array<T, Is>.size()>{})));
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

}  // namespace stationeers::ic10

#endif  // STATIONEERS_PARSER_INL
