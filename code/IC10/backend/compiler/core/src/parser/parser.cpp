// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file parser.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 16:55
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */

#include "ic10_compiler/parser/parser.hpp"
#include "common/exception/debug.hpp"
#include "common/utils/common.hpp"
#include "ic10_compiler/parser/expand_node_parser.hpp"
#include "ic10_compiler/parser/instruction_dispatcher.hpp"
#include <format>

namespace stationeers::ic10 {

    Parser::Parser(const std::vector<std::shared_ptr<Token>>& tokens, bool debug)
        : tokens_(tokens)
        , debug_(debug) {}

    Program Parser::parse() {
        auto program = Program{current()->pos};

        skip();

        while (inScope()) {
            if (current()->type == TokenType::END) [[unlikely]]
                return program;

            program.statements.push_back(parseStatement());

            // 跳过行内注释（注释与语句在同一行，不需要换行分隔）
            while (inScope() && current()->category == TokenCategory::COMMENT) consume();  // COMMENT

            // 除最后一个语句可以直接以END结尾
            if (current()->type == TokenType::END) [[unlikely]]
                break;

            // 语句之间必须以换行分隔
            if (current()->type == TokenType::NEWLINE)
                expect(TokenType::NEWLINE, false);
            else {
                reporter_.error<ICMsgId::IEP26>(current()->pos, endPos(*current()));

                gotoNextLine();  // 以换行为同步点的行级错误恢复策略
            }

            // 其余换行和注释跳过
            skip();
        }

        return program;
    }

    Program Parser::parsing(const std::vector<std::shared_ptr<Token>>& tokens, bool debug) {
        auto parser = Parser{tokens, debug};

        return parser.parse();
    }

    Statement Parser::parseStatement() {
        if (!current()) [[unlikely]] {
            reporter_.error<ICMsgId::IMP1>(current()->pos, endPos(*current()));

            return ErrorNode{*current(), ICLoc::msgStr<ICMsgId::IMP1>()};
        }

        // 指令语句交由指令分发器解析
        if (current()->type == TokenType::KEYWORD)
            return wide_cast<Statement>(parseExecutableInstruction());

        // 其余交由前瞻解析器解析
        return wide_cast<Statement>(matchVariant<FirstStatement>());
    }

    ExecutableInstruction Parser::parseExecutableInstruction() {
        if (auto c = current(); c->type == TokenType::KEYWORD && c->keyword) [[likely]] {
            consume();  // KEYWORD

            auto result = dispatch(
                // 操作数类型萃取回调，自动获取操作数类型并解析
                [&]<template<FString, OperandType...> class Ins, FString K, OperandType... Vs>(
                    Ins<K, Vs...>&&
                ) {
                    using Instruction = Ins<K, Vs...>;

                    typename Instruction::Args args;  // 指令参数tuple类型

                    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                        // 逐个解析操作数
                        ((std::get<Is>(args) = matchOperand<Vs>()), ...);
                    }(std::make_index_sequence<sizeof...(Vs)>{});

                    // 拆包参数构造指令
                    return std::apply(
                        [&](auto&&... params) { return Instruction(c->pos, params...); }, args
                    );
                },
                *c->keyword
            );

            if (result) return wide_cast<ExecutableInstruction>(*result);
        }

        reporter_.errorWith<ICMsgId::IEP3_1>(
            current()->pos, endPos(*current()), enumToStr(current()->type)
        );

        auto errToken = *current();
        consume();
        return ErrorNode{errToken, ICLoc::msgFormat<ICMsgId::IEP3_1>(enumToStr(errToken.type))};
    }

    std::optional<double> Parser::evaluateBuiltin(const std::string& name) noexcept {
        static const std::unordered_map<std::string, double> builtin = {
            {"rgas",    8.31446261815324                         },
            {"deg2rad", 0.0174532923847437                       },
            {"tau",     6.28318530717959                         },
            {"epsilon", std::numeric_limits<double>::denorm_min()},
            {"nan",     std::numeric_limits<double>::quiet_NaN() },
            {"pinf",    std::numeric_limits<double>::infinity()  },
            {"ninf",    -std::numeric_limits<double>::infinity() },
            {"pi",      3.14159265358979                         },
            {"rad2deg", 57.2957801818848                         }
        };

        if (const auto& it = builtin.find(name); it != builtin.end()) return it->second;

        return std::nullopt;
    }

    bool Parser::inScope() const noexcept {
        return idx_ < tokens_.size() && current()->type != TokenType::END;
    }

    void Parser::skip() noexcept {
        while (inScope()
               && (current()->type == TokenType::NEWLINE
                   || current()->category == TokenCategory::COMMENT))
            idx_++;

        // idx_--;
    }

    void Parser::consume() const noexcept {
        if (inScope()) [[likely]]
            idx_++;
    }

    void Parser::gotoNextLine() const noexcept {
        while (inScope() && current()->type != TokenType::NEWLINE) idx_++;

        if (inScope()) consume();
    }

    std::shared_ptr<Token> Parser::expect(TokenType type, bool skipWs, bool errorConsume) {
        if (skipWs && type != TokenType::NEWLINE) skip();

        if (inScope() && current()->type == type) return tokens_[idx_++];

        if (inScope() && errorConsume) consume();

        reporter_.errorWith<ICMsgId::IEP1_1>(
            current()->pos, endPos(*current()), enumToStr(current()->type)
        );

        throw Error{
            RuntimeError{
                         ICLoc::msgFormat<ICMsgId::IEP1_1>(enumToStr(current()->type)), current()->pos,
                         endPos(*current())
            }
        };
    }

    std::shared_ptr<Token> Parser::current(const bool consume) const noexcept {
        if (idx_ < tokens_.size()) return tokens_[consume ? idx_++ : idx_];

        return nullptr;
    }

    std::shared_ptr<Token> Parser::peek(std::size_t offset) const noexcept {
        if (idx_ + offset < tokens_.size()) return tokens_[idx_ + offset];

        return nullptr;
    }

    // LabelDef

    ShallowErrorable<LabelDef> NodeParser<LabelDef>::parse(Parser& p) {
        LabelDef labelDef{p.current()->pos};

        labelDef.identifier = NodeParser<Identifier>::parse(p);

        try {
            p.expect(TokenType::COLON, false, false);

        } catch (const Error&) {
            p.reporter_.error<ICMsgId::IEP23>(p.current()->pos, endPos(*p.current()));

            auto errToken = *p.current();
            // 不消耗 token，让 parse() 主循环通过 NEWLINE 同步到下一行
            return ErrorNode{labelDef.position, errToken, ICLoc::msgStr<ICMsgId::IEP23>()};
        }

        return labelDef;
    }

    // AliasDirective

    AliasDirective NodeParser<AliasDirective>::parse(Parser& p) {
        AliasDirective aliasDirective{p.current()->pos};

        p.consume();  // KEYWORD_ALIAS

        aliasDirective.identifier = p.match<Identifier>();

        aliasDirective.registerOrDevice = p.matchOperand<OperandType::REG_OR_DEV>();

        if (p.inScope() && p.current()->type == TokenType::TYPE_HINT_PREFIX)
            aliasDirective.typeHint = NodeParser<TypeHint>::parse(p);

        return aliasDirective;
    }

    // DefineDirective

    DefineDirective NodeParser<DefineDirective>::parse(Parser& p) {
        DefineDirective defineDirective{p.current()->pos};

        p.consume();  // KEYWORD_DEFINE

        defineDirective.identifier = p.match<Identifier>();

        // 允许标准库以字符串形式定义难以数值表示的常量，通过TypeHint的builtin标签和String类型启用
        std::optional<String> builtinValue = std::nullopt;

        // 字符串
        if (p.inScope() && p.current()->type == TokenType::STRING)  // 预检String
            builtinValue = NodeParser<String>::parse(p);
        // 数字
        else
            defineDirective.operand = p.matchOperand<OperandType::CONST_NUM>();

        // 尝试解析TypeHint
        if (p.inScope() && p.current()->type == TokenType::TYPE_HINT_PREFIX)  // 预检TypeHint
            defineDirective.typeHint = NodeParser<TypeHint>::parse(p);

        // 如果TypeHint标示为builtin，则进行预定义的浮点常量转换
        if (builtinValue && defineDirective.typeHint) {
            if (defineDirective.typeHint->builtin) {
                if (auto value = p.evaluateBuiltin(builtinValue->value.substr(1, builtinValue->value.size() - 2)); value)
                    defineDirective.operand = Float{builtinValue->position, std::to_string(*value)};

                else
                    p.reporter_.errorWith<ICMsgId::IEP32_1>(p.current()->pos, endPos(*p.current()), builtinValue->value);

            }
            else
                p.reporter_.error<ICMsgId::IEP31>(p.current()->pos, endPos(*p.current()));
        }

        return defineDirective;
    }

}  // namespace stationeers::ic10
