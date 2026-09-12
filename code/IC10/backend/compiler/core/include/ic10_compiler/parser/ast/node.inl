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
 * @if zh
 * @brief AST 节点基类的模板实现
 * @details 实现 @ref AST 与 @c LeafNode 两个模板基类共用的序列化与定位设施：@c process 把变体、可选值、字符串与算术类型统一归约为 JSON 片段，@c seqJSON 拼接 JSON 数组，@c jsonBase 组装 @c nodeName/@c position/@c end 前缀；@c LeafNode::end() 对字符串值按内容长度推进结束位置，其他值退化为起始位置。
 *          所有具体节点都复用这些设施，因此 JSON 输出的整体形状在此处统一，而非散落在各节点实现中。
 *
 * @note @c jsonBase 对算术类型刻意保持原类型而不转成字符串，使 @c toJson 能走 @c std::is_arithmetic_v 分支输出 JSON 数字，而不是带引号的字符串。
 *
 * @see node.hpp AST 节点声明
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @elseif en
 * @brief Template implementation of the AST node base classes
 * @details Implements the serialization and position facilities shared by the @ref AST and @c LeafNode template bases: @c process reduces variants, optionals, strings and arithmetic types to JSON fragments, @c seqJSON joins a JSON array, and @c jsonBase assembles the @c nodeName/@c position/@c end prefix; @c LeafNode::end() advances the end position by the string value's length and falls back to the start position for any other value.
 *          Every concrete node reuses these, so the overall shape of the JSON output is defined here rather than scattered across node implementations.
 *
 * @note @c jsonBase deliberately preserves arithmetic types instead of stringifying them, so @c toJson takes its @c std::is_arithmetic_v branch and emits a JSON number rather than a quoted string.
 *
 * @see node.hpp AST node declarations
 *
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @endif
 * */
#ifndef IC10_COMPILER_CORE_NODE_INL
#define IC10_COMPILER_CORE_NODE_INL
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

        else if constexpr (std::is_same_v<U, JsonRaw>)
            return std::forward<T>(arg).str;

        else if constexpr (JsonStringAble<U>)
            return arg;

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
        return toJson<"nodeName", "position", "end", Vs...>(
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

#endif  // IC10_COMPILER_CORE_NODE_INL
