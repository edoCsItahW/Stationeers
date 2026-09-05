// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file token_adapter.cpp
 * @author edocsitahw
 * @version 1.0
 * @date 2026/07/01
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10_compiler_python/lexer/token_adapter.hpp"
#include "ic10_compiler/pch/pch.hpp"
#include "common_python/enum_adapter.hpp"

namespace stationeers::ic10 {

    void initToken(py::module_& m) {
        // TODO: 需重新编译验证 — 改用编译期反射导出枚举，与 C++ 定义自动同步
        stationeers::exportEnum<TokenType>(m, "TokenType");
        stationeers::exportEnum<TokenCategory>(m, "TokenCategory");
        stationeers::exportEnum<InstructionKeyword>(m, "InstructionKeyword");

        // 绑定Token类(使用shared_ptr持有)
        // Bind Token class(with shared_ptr holder)
        py::class_<Token, std::shared_ptr<Token>>(m, "Token")
            .def(py::init<>([]() {
                return std::make_shared<Token>();
            }))
            .def(py::init<>([](TokenType type, const Pos& pos,
                               py::object lexeme, py::object category) {
                auto t = std::make_shared<Token>();
                t->type = type;
                t->pos = pos;
                if (!lexeme.is_none()) t->lexeme = lexeme.cast<std::string>();
                if (!category.is_none()) t->category = category.cast<TokenCategory>();
                return t;
            }), py::arg("type"), py::arg("pos"),
                py::arg("lexeme") = py::none(), py::arg("category") = py::none())
            /// @if zh 标记类型 @elseif en Token type @endif
            .def_readwrite("type", &Token::type)
            /// @if zh 标记位置 @elseif en Token position @endif
            .def_readwrite("pos", &Token::pos)
            /// @if zh 标记词素 @elseif en Token lexeme @endif
            .def_readwrite("lexeme", &Token::lexeme)
            /// @if zh 标记类别 @elseif en Token category @endif
            .def_readwrite("category", &Token::category)
            /// @if zh 指令关键字(仅 KEYWORD 类型有效) @elseif en Instruction keyword @endif
            .def_property_readonly("keyword", [](const Token& self) -> py::object {
                if (self.keyword)
                    return py::cast(static_cast<int>(*self.keyword));
                return py::none();
            })
            /**
             * @if zh
             * @brief 转换为字符串表示
             * @return 人类可读的字符串描述
             * @elseif en
             * @brief Convert to string representation
             * @return Human-readable string description
             * @endif
             */
            .def("toString", &Token::toString)
            /**
             * @if zh
             * @brief 转换为JSON格式
             * @return JSON格式字符串
             * @elseif en
             * @brief Convert to JSON format
             * @return JSON format string
             * @endif
             */
            .def("toJSON", &Token::toJSON)
            .def("__repr__", [](const Token& self) {
                return self.toString();
            });
    }

}  // namespace stationeers::ic10
