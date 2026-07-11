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

namespace stationeers::ic10 {

    // 分析入口：构造临时分析器并访问程序根节点
    Task<> Analyser::analyse(const Program& program) {
        auto analyser = Analyser();

        (void)co_await analyser.visit(program);
    }

    SymbolTable& Analyser::getSymbolTable() { return symbolTable_; }

    // 获取诊断报告器累积的所有诊断信息
    const std::vector<Diagnostic>& Analyser::getDiagnostics() const {
        return reporter_.getDiagnostics();
    }

    // 访问 Program：逐条遍历语句，结束后清理未决 Future
    Task<> Analyser::visit(const Program& program) {
        for (const auto& stmt : program.statements)
            std::visit(
                [this]<typename T>(const T& arg) -> Task<> {
                    (void)co_await this->operator()(arg);
                },
                stmt
            );

        // 分析结束，此时依然悬而未决的 Future 被确定为未定义，向所有等待者返回错误
        symbolTable_.failAllPending();

        co_return;
    }

    // 解析符号：从符号表取 Future 并等待结果，失败则转化为诊断
    Task<std::shared_ptr<Symbol>> Analyser::resolveSymbol(const std::string& name, const Pos& pos) {
        auto result = co_await std::move(symbolTable_.resolve(name, pos));

        // 解析失败：将异常重新抛出以捕获其消息，统一以 IE0_1 上报
        if (!result.has_value()) {
            try {
                std::rethrow_exception(result.error());
            } catch (const Error& e) {
                reporter_.errorWith<MsgId::IE0_1>(e.getStart(), e.getEnd(), e.message());
            } catch (const std::exception& e) {
                reporter_.errorWith<MsgId::IE0_1>(
                    pos, endPos(pos, name.size()), std::string(e.what())
                );
            }

            co_return nullptr;
        }

        co_return result.value();
    }

    // 定义符号：包装符号表 define，重定义时上报 IEA2_1
    void Analyser::defineSymbol(const Identifier& identifier, Symbol&& symbol) {
        if (auto res = symbolTable_.define(identifier.value, std::make_shared<Symbol>(symbol));
            !res.has_value())
            reporter_.errorWith<MsgId::IEA2_1>(
                identifier.start(), identifier.end(), identifier.value
            );
    }

    // 标签定义：将标签名定义为 INTEGER 符号
    Task<> Analyser::operator()(const LabelDef& labelDef) {
        // Identifier: 定义为新符号
        if (std::holds_alternative<Identifier>(labelDef.identifier)) {
            const auto identifier = std::get<Identifier>(labelDef.identifier);

            defineSymbol(identifier, {Type::INTEGER, identifier.value});
        }

        // ErrorNode: identifier 解析失败，上报类型不匹配
        else
            reporter_.errorWith<MsgId::IEA1_2>(
                labelDef.start(), labelDef.end(), Identifier::nodeName.value.data(),
                std::get<ErrorNode>(labelDef.identifier).nodeName.value.data()
            );

        co_return;
    }

    // alias 指令：为寄存器或设备定义别名
    Task<> Analyser::operator()(const AliasDirective& aliasDirective) {
        // 若 registerOrDevice 为 Identifier，需先解析（检查是否已定义）
        // 由于 std::visit 的 lambda 是同步的，co_await 须在 visit 之前完成
        if (std::holds_alternative<Identifier>(aliasDirective.identifier) &&
            std::holds_alternative<Identifier>(aliasDirective.registerOrDevice)) {
            const auto& refId = std::get<Identifier>(aliasDirective.registerOrDevice);
            (void)co_await resolveSymbol(refId.value, refId.position);
        }

        // Identifier: 按寄存器/设备类型定义别名
        if (std::holds_alternative<Identifier>(aliasDirective.identifier)) {
            const auto identifier = std::get<Identifier>(aliasDirective.identifier);

            defineSymbol(
                identifier, {std::visit(
                                 [&]<typename U>(U&&) -> Type {
                                     using V = std::decay_t<U>;

                                     // 不允许为别名定义别名
                                     if constexpr (std::is_same_v<V, Identifier>) {
                                         reporter_.error<MsgId::IEA4>(
                                             aliasDirective.start(), aliasDirective.end()
                                         );
                                         return Type::UNKNOWN;
                                     }

                                     // ErrorNode: Parser 已报错，此处跳过
                                     else if constexpr (std::is_same_v<V, ErrorNode>)
                                         return Type::UNKNOWN;

                                    else
                                         return type_of<V>;
                                 },
                                 aliasDirective.registerOrDevice
                             ),
                             identifier.value}
            );
        }

        // ErrorNode: identifier 解析失败，上报类型不匹配
        else
            reporter_.errorWith<MsgId::IEA1_2>(
                aliasDirective.start(), aliasDirective.end(), Identifier::nodeName.value.data(),
                std::get<ErrorNode>(aliasDirective.identifier).nodeName.value.data()
            );

        co_return;
    }

    // define 指令：定义常量符号，其值由 operand 决定
    Task<> Analyser::operator()(const DefineDirective& defineDirective) {
        // 若 operand 为 Identifier，需先解析（检查是否已定义）
        // 由于 std::visit 的 lambda 是同步的，co_await 须在 visit 之前完成
        if (std::holds_alternative<Identifier>(defineDirective.identifier) &&
            std::holds_alternative<Identifier>(defineDirective.operand)) {
            const auto& opId = std::get<Identifier>(defineDirective.operand);
            (void)co_await resolveSymbol(opId.value, opId.position);
        }

        std::visit(
            [&]<typename T>(const T& arg) {
                // Identifier: 按 operand 类型定义常量
                if constexpr (std::is_same_v<T, Identifier>)
                    defineSymbol(
                        arg, {std::visit(
                                  []<typename U>(U&&) -> Type {
                                      using V = std::decay_t<U>;

                                      // ErrorNode: Parser 已报错，此处跳过
                                      if constexpr (std::is_same_v<V, ErrorNode>)
                                          return Type::UNKNOWN;

                                      return type_of<V>;
                                  },
                                  defineDirective.operand
                              ),
                              arg.value}
                    );

                // ErrorNode: identifier 解析失败，上报类型不匹配
                else
                    reporter_.errorWith<MsgId::IEA1_2>(
                        defineDirective.start(), defineDirective.end(),
                        Identifier::nodeName.value.data(), arg.nodeName.value.data()
                    );
            },
            defineDirective.identifier
        );

        co_return;
    }

    // STR 宏调用：仅检查 value 是否为 ErrorNode
    Task<> Analyser::operator()(const StrCall& strCall) {
        // value 解析失败，上报类型不匹配
        if (std::holds_alternative<ErrorNode>(strCall.value))
            reporter_.errorWith<MsgId::IEA1_2>(
                strCall.start(), strCall.end(), String::nodeName.value.data(),
                std::get<ErrorNode>(strCall.value).nodeName.value.data()
            );

        co_return;
    }

    // HASH 宏调用：仅检查 value 是否为 ErrorNode
    Task<> Analyser::operator()(const HashCall& hashCall) {
        // value 解析失败，上报类型不匹配
        if (std::holds_alternative<ErrorNode>(hashCall.value))
            reporter_.errorWith<MsgId::IEA1_2>(
                hashCall.start(), hashCall.end(), String::nodeName.value.data(),
                std::get<ErrorNode>(hashCall.value).nodeName.value.data()
            );

        co_return;
    }

    // 以下为叶节点访问器：这些节点无子节点需要遍历，也无需符号解析，直接返回

    Task<> Analyser::operator()(const Constant& constant) { co_return; }

    Task<> Analyser::operator()(const Device& device) { co_return; }

    Task<> Analyser::operator()(const Register& register_) { co_return; }

    Task<> Analyser::operator()(const String& string) { co_return; }

    // Identifier 叶节点访问器：保留作为防御性兜底
    // 正常流程中 Identifier 会在指令访问器内被直接 resolveSymbol，不会走到此分支
    Task<> Analyser::operator()(const Identifier& identifier) { co_return; }

    Task<> Analyser::operator()(const BinaryNumber& binaryNumber) { co_return; }

    Task<> Analyser::operator()(const HexNumber& hexNumber) { co_return; }

    Task<> Analyser::operator()(const Float& float_) { co_return; }

    Task<> Analyser::operator()(const Integer& integer) { co_return; }

    // ErrorNode 叶节点访问器：Parser 已上报，此处跳过避免重复诊断
    Task<> Analyser::operator()(const ErrorNode& errorNode) { co_return; }

}  // namespace stationeers::ic10
