// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file analyser.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/06/05 17:50
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "ic10/semantic/analyser.hpp"
#include "ic10/locals/local.hpp"
#include <iostream>

namespace stationeers::ic10 {

    Task<> Analyser::analyse(const Program& program) {
        auto analyser = Analyser();

        (void)co_await analyser.visit(program);
    }

    SymbolTable& Analyser::getSymbolTable() { return symbolTable_; }

    std::vector<Error>& Analyser::getErrors() { return errors_; }

    Task<> Analyser::visit(const Program& program) {
        for (const auto& stmt : program.statements)
            std::visit(
                [this]<typename T>(const T& arg) -> Task<> {
                    (void)co_await this->operator()(arg);
                },
                stmt
            );

        // 分析结束，此时依然悬而未决的Future被确定为未定义，向所有等待着返回错误
        symbolTable_.failAllPending();

        co_return;
    }

    Task<std::shared_ptr<Symbol>> Analyser::resolveSymbol(const std::string& name, const Pos& pos) {
        auto result = co_await std::move(symbolTable_.resolve(name, pos));

        if (!result.has_value()) {
            try {
                std::rethrow_exception(result.error());
            } catch (const Error& e) { errors_.push_back(e); } catch (const std::exception& e) {
                reportError<RuntimeError>(
                    std::string(e.what()), pos, endPos(pos, name.size()),
                    std::source_location::current()
                );
            }

            co_return nullptr;
        }

        co_return result.value();
    }

    void Analyser::defineSymbol(const Identifier& identifier, Symbol&& symbol) {
        if (auto res = symbolTable_.define(identifier.value, std::make_shared<Symbol>(symbol));
            !res.has_value())
            reportError<RedefinitionError>(res.error(), identifier.start(), identifier.end());
    }

    Task<> Analyser::operator()(const LabelDef& labelDef) {
        // Identifier
        if (std::holds_alternative<Identifier>(labelDef.identifier)) {
            const auto identifier = std::get<Identifier>(labelDef.identifier);

            defineSymbol(identifier, {Type::INTEGER, identifier.value});
        }

        co_return;
    }

    Task<> Analyser::operator()(const AliasDirective& aliasDirective) {
        // Identifier
        if (std::holds_alternative<Identifier>(aliasDirective.identifier)) {
            const auto identifier = std::get<Identifier>(aliasDirective.identifier);

            defineSymbol(
                identifier, {std::visit(
                                 [&]<typename U>(U&&) {
                                     using V = std::decay_t<U>;

                                     // 不允许为别名定义别名
                                     if constexpr (std::is_same_v<V, Identifier>)
                                         reportError<SyntaxError>(
                                             Loc::msg<MsgId::IEA4>(), aliasDirective.start(),
                                             aliasDirective.end()
                                         );

                                     return type_of<V>;
                                 },
                                 aliasDirective.registerOrDevice
                             ),
                             identifier.value}
            );
        }

        // ErrorNode
        else
            reportError<TypeError>(
                Loc::msgFormat<MsgId::IEA1_2>(
                    Identifier::nodeName.value.data(),
                    std::get<ErrorNode>(aliasDirective.identifier).nodeName.value.data()
                ),
                aliasDirective.start(), aliasDirective.end()
            );

        co_return;
    }

    Task<> Analyser::operator()(const DefineDirective& defineDirective) {
        std::visit(
            [&]<typename T>(const T& arg) {
                // Identifier
                if constexpr (std::is_same_v<T, Identifier>)
                    defineSymbol(
                        arg, {std::visit(
                                  []<typename U>(U&&) { return type_of<std::decay_t<U>>; },
                                  defineDirective.operand
                              ),
                              arg.value}
                    );

                // ErrorNode
                else
                    reportError<TypeError>(
                        Loc::msgFormat<MsgId::IEA1_2>(
                            Identifier::nodeName.value.data(), arg.nodeName.value.data()
                        ),
                        defineDirective.start(), defineDirective.end()
                    );
            },
            defineDirective.identifier
        );

        co_return;
    }

    Task<> Analyser::operator()(const StrCall& strCall) {
        std::cout << "StrCall" << std::endl;

        co_return;
    }

    Task<> Analyser::operator()(const HashCall& hashCall) {
        std::cout << "HashCall" << std::endl;

        co_return;
    }

    Task<> Analyser::operator()(const Constant& constant) {
        std::cout << "Constant" << std::endl;

        co_return;
    }

    Task<> Analyser::operator()(const Device& device) {
        std::cout << "Device" << std::endl;

        co_return;
    }

    Task<> Analyser::operator()(const Register& register_) {
        std::cout << "Register" << std::endl;

        co_return;
    }

    Task<> Analyser::operator()(const String& string) {
        std::cout << "String" << std::endl;

        co_return;
    }

    Task<> Analyser::operator()(const Identifier& identifier) {
        std::cout << "Identifier" << std::endl;

        co_return;
    }

    Task<> Analyser::operator()(const BinaryNumber& binaryNumber) {
        std::cout << "BinaryNumber" << std::endl;

        co_return;
    }

    Task<> Analyser::operator()(const HexNumber& hexNumber) {
        std::cout << "HexNumber" << std::endl;

        co_return;
    }

    Task<> Analyser::operator()(const Float& float_) {
        std::cout << "Float" << std::endl;

        co_return;
    }

    Task<> Analyser::operator()(const Integer& integer) {
        std::cout << "Integer" << std::endl;

        co_return;
    }

    Task<> Analyser::operator()(const ErrorNode& errorNode) {
        std::cout << "ErrorNode" << std::endl;

        co_return;
    }

}  // namespace stationeers::ic10
