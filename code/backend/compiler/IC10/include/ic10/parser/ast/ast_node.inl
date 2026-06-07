// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: 2207150234@st.sziit.edu.cn

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
    template<typename... Ts>
    std::string AST<Derived>::jsonBase(std::pair<std::string, Ts>... fields) const {
        std::stringstream ss;

        ss << R"({ "type": ")" << std::string(Derived::nodeName) << R"(", "position": { "line": )"
           << position.line() << R"(, "column": )" << position.column() << R"( })";

        ((ss << R"(, ")" << fields.first << R"(": )" <<
          [&] {
              if constexpr (IsVariant<Ts>)
                  return call(fields.second, [](auto&& o) { return o.toJSON(); });

              else if constexpr (std::is_arithmetic_v<Ts>)
                  return std::to_string(fields.second);

              else
                  return fields.second;
          }()),
         ...);

        ss << R"( } )";

        return ss.str();
    }

}  // namespace stationeers::ic10

#endif  // COMPILER_AST_NODE_INL
