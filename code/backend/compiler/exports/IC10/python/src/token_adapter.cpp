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
#include "token_adapter.hpp"
#include "ic10/lexer/token.hpp"

namespace stationeers::ic10 {

    void initToken(py::module_& m) {
        // 绑定TokenType枚举
        // Bind TokenType enum
        py::enum_<TokenType>(m, "TokenType")
            .value("INTEGER", TokenType::INTEGER)
            .value("FLOAT", TokenType::FLOAT)
            .value("HEX_NUMBER", TokenType::HEX_NUMBER)
            .value("BINARY_NUMBER", TokenType::BINARY_NUMBER)
            .value("STRING", TokenType::STRING)
            .value("IDENTIFIER", TokenType::IDENTIFIER)
            .value("REGISTER", TokenType::REGISTER)
            .value("DEVICE", TokenType::DEVICE)
            .value("LPAREN", TokenType::LPAREN)
            .value("RPAREN", TokenType::RPAREN)
            .value("COLON", TokenType::COLON)
            .value("HEX_COMMENT", TokenType::HEX_COMMENT)
            .value("SLASH_COMMENT", TokenType::SLASH_COMMENT)
            .value("NEWLINE", TokenType::NEWLINE)
            .value("KEYWORD_HASH", TokenType::KEYWORD_HASH)
            .value("KEYWORD_STR", TokenType::KEYWORD_STR)
            .value("KEYWORD_NAN", TokenType::KEYWORD_NAN)
            .value("KEYWORD_PINF", TokenType::KEYWORD_PINF)
            .value("KEYWORD_NINF", TokenType::KEYWORD_NINF)
            .value("KEYWORD_PI", TokenType::KEYWORD_PI)
            .value("KEYWORD_TAU", TokenType::KEYWORD_TAU)
            .value("KEYWORD_DEG2RAD", TokenType::KEYWORD_DEG2RAD)
            .value("KEYWORD_RAD2DEG", TokenType::KEYWORD_RAD2DEG)
            .value("KEYWORD_EPSILON", TokenType::KEYWORD_EPSILON)
            .value("KEYWORD_GAS_CONSTANT", TokenType::KEYWORD_GAS_CONSTANT)
            .value("END", TokenType::END)
            .value("UNKNOWN", TokenType::UNKNOWN)
            .value("KEYWORD_HCF", TokenType::KEYWORD_HCF)
            .value("KEYWORD_YIELD", TokenType::KEYWORD_YIELD)
            .value("KEYWORD_ALIAS", TokenType::KEYWORD_ALIAS)
            .value("KEYWORD_DEFINE", TokenType::KEYWORD_DEFINE)
            .value("KEYWORD_PEEK", TokenType::KEYWORD_PEEK)
            .value("KEYWORD_POP", TokenType::KEYWORD_POP)
            .value("KEYWORD_PUSH", TokenType::KEYWORD_PUSH)
            .value("KEYWORD_CLR", TokenType::KEYWORD_CLR)
            .value("KEYWORD_J", TokenType::KEYWORD_J)
            .value("KEYWORD_JAL", TokenType::KEYWORD_JAL)
            .value("KEYWORD_JR", TokenType::KEYWORD_JR)
            .value("KEYWORD_RAND", TokenType::KEYWORD_RAND)
            .value("KEYWORD_SLEEP", TokenType::KEYWORD_SLEEP)
            .value("KEYWORD_ABS", TokenType::KEYWORD_ABS)
            .value("KEYWORD_ACOS", TokenType::KEYWORD_ACOS)
            .value("KEYWORD_ADD", TokenType::KEYWORD_ADD)
            .value("KEYWORD_ASIN", TokenType::KEYWORD_ASIN)
            .value("KEYWORD_ATAN", TokenType::KEYWORD_ATAN)
            .value("KEYWORD_ATAN2", TokenType::KEYWORD_ATAN2)
            .value("KEYWORD_CEIL", TokenType::KEYWORD_CEIL)
            .value("KEYWORD_COS", TokenType::KEYWORD_COS)
            .value("KEYWORD_DIV", TokenType::KEYWORD_DIV)
            .value("KEYWORD_EXP", TokenType::KEYWORD_EXP)
            .value("KEYWORD_FLOOR", TokenType::KEYWORD_FLOOR)
            .value("KEYWORD_LOG", TokenType::KEYWORD_LOG)
            .value("KEYWORD_MAX", TokenType::KEYWORD_MAX)
            .value("KEYWORD_MIN", TokenType::KEYWORD_MIN)
            .value("KEYWORD_MOD", TokenType::KEYWORD_MOD)
            .value("KEYWORD_MUL", TokenType::KEYWORD_MUL)
            .value("KEYWORD_POW", TokenType::KEYWORD_POW)
            .value("KEYWORD_ROUND", TokenType::KEYWORD_ROUND)
            .value("KEYWORD_SIN", TokenType::KEYWORD_SIN)
            .value("KEYWORD_SQRT", TokenType::KEYWORD_SQRT)
            .value("KEYWORD_SUB", TokenType::KEYWORD_SUB)
            .value("KEYWORD_TAN", TokenType::KEYWORD_TAN)
            .value("KEYWORD_TRUNC", TokenType::KEYWORD_TRUNC)
            .value("KEYWORD_NOT", TokenType::KEYWORD_NOT)
            .value("KEYWORD_MOVE", TokenType::KEYWORD_MOVE)
            .value("KEYWORD_POKE", TokenType::KEYWORD_POKE)
            .value("KEYWORD_BEQZ", TokenType::KEYWORD_BEQZ)
            .value("KEYWORD_BEQZAL", TokenType::KEYWORD_BEQZAL)
            .value("KEYWORD_BNEZ", TokenType::KEYWORD_BNEZ)
            .value("KEYWORD_BNEZAL", TokenType::KEYWORD_BNEZAL)
            .value("KEYWORD_BGEZ", TokenType::KEYWORD_BGEZ)
            .value("KEYWORD_BGEZAL", TokenType::KEYWORD_BGEZAL)
            .value("KEYWORD_BGTZ", TokenType::KEYWORD_BGTZ)
            .value("KEYWORD_BGTZAL", TokenType::KEYWORD_BGTZAL)
            .value("KEYWORD_BLEZ", TokenType::KEYWORD_BLEZ)
            .value("KEYWORD_BLEZAL", TokenType::KEYWORD_BLEZAL)
            .value("KEYWORD_BLTZ", TokenType::KEYWORD_BLTZ)
            .value("KEYWORD_BLTZAL", TokenType::KEYWORD_BLTZAL)
            .value("KEYWORD_BNAN", TokenType::KEYWORD_BNAN)
            .value("KEYWORD_BNANZ", TokenType::KEYWORD_BNANZ)
            .value("KEYWORD_BDNS", TokenType::KEYWORD_BDNS)
            .value("KEYWORD_BDNSAL", TokenType::KEYWORD_BDNSAL)
            .value("KEYWORD_BDSE", TokenType::KEYWORD_BDSE)
            .value("KEYWORD_BDSEAL", TokenType::KEYWORD_BDSEAL)
            .value("KEYWORD_BREQZ", TokenType::KEYWORD_BREQZ)
            .value("KEYWORD_BRNZ", TokenType::KEYWORD_BRNZ)
            .value("KEYWORD_BRGEZ", TokenType::KEYWORD_BRGEZ)
            .value("KEYWORD_BRGTZ", TokenType::KEYWORD_BRGTZ)
            .value("KEYWORD_BRLEZ", TokenType::KEYWORD_BRLEZ)
            .value("KEYWORD_BRLTZ", TokenType::KEYWORD_BRLTZ)
            .value("KEYWORD_BRNAN", TokenType::KEYWORD_BRNAN)
            .value("KEYWORD_BRDNS", TokenType::KEYWORD_BRDNS)
            .value("KEYWORD_BRDSE", TokenType::KEYWORD_BRDSE)
            .value("KEYWORD_SEQZ", TokenType::KEYWORD_SEQZ)
            .value("KEYWORD_SNEZ", TokenType::KEYWORD_SNEZ)
            .value("KEYWORD_SGEZ", TokenType::KEYWORD_SGEZ)
            .value("KEYWORD_SGTZ", TokenType::KEYWORD_SGTZ)
            .value("KEYWORD_SLEZ", TokenType::KEYWORD_SLEZ)
            .value("KEYWORD_SLTZ", TokenType::KEYWORD_SLTZ)
            .value("KEYWORD_SNAN", TokenType::KEYWORD_SNAN)
            .value("KEYWORD_SNANZ", TokenType::KEYWORD_SNANZ)
            .value("KEYWORD_SDNS", TokenType::KEYWORD_SDNS)
            .value("KEYWORD_SDSE", TokenType::KEYWORD_SDSE)
            .value("KEYWORD_AND", TokenType::KEYWORD_AND)
            .value("KEYWORD_NOR", TokenType::KEYWORD_NOR)
            .value("KEYWORD_OR", TokenType::KEYWORD_OR)
            .value("KEYWORD_SLA", TokenType::KEYWORD_SLA)
            .value("KEYWORD_SLL", TokenType::KEYWORD_SLL)
            .value("KEYWORD_SRA", TokenType::KEYWORD_SRA)
            .value("KEYWORD_SRL", TokenType::KEYWORD_SRL)
            .value("KEYWORD_XOR", TokenType::KEYWORD_XOR)
            .value("KEYWORD_GET", TokenType::KEYWORD_GET)
            .value("KEYWORD_PUT", TokenType::KEYWORD_PUT)
            .value("KEYWORD_L", TokenType::KEYWORD_L)
            .value("KEYWORD_LS", TokenType::KEYWORD_LS)
            .value("KEYWORD_LR", TokenType::KEYWORD_LR)
            .value("KEYWORD_S", TokenType::KEYWORD_S)
            .value("KEYWORD_SB", TokenType::KEYWORD_SB)
            .value("KEYWORD_RMAP", TokenType::KEYWORD_RMAP)
            .value("KEYWORD_BEQ", TokenType::KEYWORD_BEQ)
            .value("KEYWORD_BEQAL", TokenType::KEYWORD_BEQAL)
            .value("KEYWORD_BNE", TokenType::KEYWORD_BNE)
            .value("KEYWORD_BNEAL", TokenType::KEYWORD_BNEAL)
            .value("KEYWORD_BGE", TokenType::KEYWORD_BGE)
            .value("KEYWORD_BGEAL", TokenType::KEYWORD_BGEAL)
            .value("KEYWORD_BGT", TokenType::KEYWORD_BGT)
            .value("KEYWORD_BGTAL", TokenType::KEYWORD_BGTAL)
            .value("KEYWORD_BLE", TokenType::KEYWORD_BLE)
            .value("KEYWORD_BLEAL", TokenType::KEYWORD_BLEAL)
            .value("KEYWORD_BLT", TokenType::KEYWORD_BLT)
            .value("KEYWORD_BLTAL", TokenType::KEYWORD_BLTAL)
            .value("KEYWORD_BAPZ", TokenType::KEYWORD_BAPZ)
            .value("KEYWORD_BAPZAL", TokenType::KEYWORD_BAPZAL)
            .value("KEYWORD_BNAZ", TokenType::KEYWORD_BNAZ)
            .value("KEYWORD_BNAZAL", TokenType::KEYWORD_BNAZAL)
            .value("KEYWORD_BDNVL", TokenType::KEYWORD_BDNVL)
            .value("KEYWORD_BDNVS", TokenType::KEYWORD_BDNVS)
            .value("KEYWORD_BREQ", TokenType::KEYWORD_BREQ)
            .value("KEYWORD_BRNE", TokenType::KEYWORD_BRNE)
            .value("KEYWORD_BRGE", TokenType::KEYWORD_BRGE)
            .value("KEYWORD_BRGT", TokenType::KEYWORD_BRGT)
            .value("KEYWORD_BRLE", TokenType::KEYWORD_BRLE)
            .value("KEYWORD_BRLT", TokenType::KEYWORD_BRLT)
            .value("KEYWORD_BRAPZ", TokenType::KEYWORD_BRAPZ)
            .value("KEYWORD_BRNAZ", TokenType::KEYWORD_BRNAZ)
            .value("KEYWORD_SAPZ", TokenType::KEYWORD_SAPZ)
            .value("KEYWORD_SNAZ", TokenType::KEYWORD_SNAZ)
            .value("KEYWORD_SEQ", TokenType::KEYWORD_SEQ)
            .value("KEYWORD_SNE", TokenType::KEYWORD_SNE)
            .value("KEYWORD_SGE", TokenType::KEYWORD_SGE)
            .value("KEYWORD_SGT", TokenType::KEYWORD_SGT)
            .value("KEYWORD_SLE", TokenType::KEYWORD_SLE)
            .value("KEYWORD_SLT", TokenType::KEYWORD_SLT)
            .value("KEYWORD_LERP", TokenType::KEYWORD_LERP)
            .value("KEYWORD_EXT", TokenType::KEYWORD_EXT)
            .value("KEYWORD_INS", TokenType::KEYWORD_INS)
            .value("KEYWORD_SS", TokenType::KEYWORD_SS)
            .value("KEYWORD_LB", TokenType::KEYWORD_LB)
            .value("KEYWORD_SBN", TokenType::KEYWORD_SBN)
            .value("KEYWORD_SBS", TokenType::KEYWORD_SBS)
            .value("KEYWORD_BAP", TokenType::KEYWORD_BAP)
            .value("KEYWORD_BAPAL", TokenType::KEYWORD_BAPAL)
            .value("KEYWORD_BNA", TokenType::KEYWORD_BNA)
            .value("KEYWORD_BNAAL", TokenType::KEYWORD_BNAAL)
            .value("KEYWORD_BRAP", TokenType::KEYWORD_BRAP)
            .value("KEYWORD_BRNA", TokenType::KEYWORD_BRNA)
            .value("KEYWORD_SAP", TokenType::KEYWORD_SAP)
            .value("KEYWORD_SNA", TokenType::KEYWORD_SNA)
            .value("KEYWORD_SELECT", TokenType::KEYWORD_SELECT)
            .value("KEYWORD_LBN", TokenType::KEYWORD_LBN)
            .value("KEYWORD_LBS", TokenType::KEYWORD_LBS)
            .value("KEYWORD_LBNS", TokenType::KEYWORD_LBNS)
            .export_values();

        // 绑定TokenCategory枚举
        // Bind TokenCategory enum
        py::enum_<TokenCategory>(m, "TokenCategory")
            .value("LITERAL", TokenCategory::LITERAL)
            .value("SYMBOL", TokenCategory::SYMBOL)
            .value("COMMENT", TokenCategory::COMMENT)
            .value("WHITESPACE", TokenCategory::WHITESPACE)
            .value("END", TokenCategory::END)
            .value("INVALID", TokenCategory::INVALID)
            .export_values();

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
