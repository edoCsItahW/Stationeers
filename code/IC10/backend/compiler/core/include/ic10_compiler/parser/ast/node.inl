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
    AST<Derived>::AST(Pos pos) noexcept
        : position{pos} {}

    template<typename Derived>
    Pos AST<Derived>::start() const noexcept {
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
        return toJson<"type", "position", "end", Vs...>(
            Derived::nodeName, position.toJSON(), end().toJSON(), [](auto&& arg) -> decltype(auto) {
                using U = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<U, std::nullopt_t>)
                    return std::optional<std::string>(std::nullopt);
                else if constexpr (IsOptional<U>)
                    if (!arg.has_value())
                        return std::optional<std::string>(std::nullopt);
                    else
                        return std::optional<std::string>(process(*arg));
                else if constexpr (std::is_arithmetic_v<U>)
                    return U(
                        arg
                    );  // 保持算术类型，toJson 通过 std::is_arithmetic_v 分支直接输出 JSON 数字
                else
                    return process(arg);
            }(std::forward<Args>(args))...
        );
    }

    // LeafNode

    template<typename Derived, typename Value>
    LeafNode<Derived, Value>::LeafNode(Pos pos, Value val)
        : AST<Derived>{pos}
        , value(std::move(val)) {}

    template<typename Derived, typename Value>
    Pos LeafNode<Derived, Value>::end() const {
        if constexpr (std::is_same_v<Value, std::string>)
            return endPos(AST<Derived>::position, value);

        else
            return AST<LeafNode>::position;
    }

    template<typename Derived, typename Value>
    std::string LeafNode<Derived, Value>::toString() const {
        if constexpr (requires {std::to_string(value); } )
            return std::to_string(value);

        else
            return value;
    }

    template<typename Derived, typename Value>
    std::string LeafNode<Derived, Value>::toJSON() const {
        return  AST<Derived>::template jsonBase<"value">(value);
    }

}  // namespace stationeers::ic10

#endif  // COMPILER_AST_NODE_INL
